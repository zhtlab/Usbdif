/*
 * Copyright (c) 2018 zhtlab
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define _USBDHID_C_

#include        "usb_def.h"
#include        "usbdif.h"
#include        "usb_desc.h"

#include        "rtos.h"

#include        "usbdhid.h"

uint8_t         usbdhidBufRx[64];

extern struct _stUsbdif        usbdif;

usbifClassCb_t  usbdHidClassCb = {
  UsbdHidInit,
  UsbdHidDeInit,
  UsbdHidSetup,
  UsbdHidDataInDone,
  UsbdHidDataOut,
  USBDIF_NULL,
};


static usbdifStatus_t
UsbdHidInit(usbdifClassDef_t *prc, int cfgnum)
{
  uint8_t ret = USBDIF_STATUS_UNKNOWN;

  usbdHidHandle_t               *pHid;
  int                           szIn, szOut;
  int                           speed;

  speed = usbdif.sc[prc->dev].speed;

  switch(speed) {
  case  USBIF_SPEED_FULL:
    szIn  = USBDHID_PACKET_SIZE_FS;
    szOut = USBDHID_PACKET_SIZE_FS;
    break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szIn  = USBDHID_PACKET_SIZE_HS;
    szOut = USBDHID_PACKET_SIZE_HS;
  }

  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdHidHandle_t));
    if(prc->pClassData == USBDIF_NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(sizeof (usbdHidHandle_t)));
  }

  pHid = (usbdHidHandle_t*) prc->pClassData;

  pHid->epIn   = USBDESC_EP_HID_IN;
  //pHid->epOut  = USBDESC_EP_HID_OUT;

  UsbdevOpenEp(prc->dev, pHid->epIn,   USBIF_EP_BULK, szIn);
  //UsbdevOpenEp(prc->dev, pHid->epOut,  USBIF_EP_BULK, szOut);

  /* Init Xfer states */
  pHid->TxState = 0;
  pHid->RxState = 0;

  if(prc->pUserData) ((usbdhidCb_t *)prc->pUserData)->init(prc);

  /* Prepare Out endpoint to receive next packet */
  pHid->RxBuffer = usbdhidBufRx;
  pHid->RxLength = sizeof(usbdhidBufRx);
  //UsbdevPrepareRecv(prc->dev, USBDESC_EP_HID_OUT, pHid->RxBuffer, pHid->RxLength);

  ret = USBDIF_STATUS_SUCCESS;

fail:
  return ret;
}


static usbdifStatus_t
UsbdHidDeInit(usbdifClassDef_t *prc, int cfgnum)
{
  uint8_t ret = 0;
  usbdHidHandle_t               *pHid;

  pHid = (usbdHidHandle_t *)prc->pClassData;

  if(prc->pUserData) ((usbdhidCb_t *)prc->pUserData)->deinit(prc);

  /* Close all EPs */
  UsbdevCloseEp(prc->dev, pHid->epIn);
  UsbdevCloseEp(prc->dev, pHid->epOut);

  /* DeInit  physical Interface components */
  if(prc->pClassData != USBDIF_NULL) {  /* adhoc need???? */
    UsbdifFree(prc->pClassData);
    prc->pClassData = USBDIF_NULL;
  }

  return ret;
}


static usbdifStatus_t
UsbdHidSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  usbdifStatus_t        result;

  usbdHidHandle_t       *pHid;
  pHid = (usbdHidHandle_t*) prc->pClassData;

  /* check to registered the pointer of the hid class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  if(s->bmRequest == 0x21) {            /* Out, Class, Interface */
    if(s->bRequest == USB_BREQ_HID_SET_IDLE) {
      result = USBDIF_STATUS_SUCCESS;
    } else if(s->bRequest == USB_BREQ_HID_SET_OUTPUT_REPORT) {
      pHid->enOutputReport = s->buf[0];
      result = USBDIF_STATUS_SUCCESS;
    }

  } else if(s->bmRequest == 0x81) {     /* In,  Standard, Interface */
    switch(s->bRequest) {
    case        USB_BREQ_GET_DESCRIPTOR:
      if((s->wValue >> 8) == USB_DESC_HID_REPORT) {
        /* get the report */
        s->len = s->wLength;
        if(prc->pUserData) {
          ((usbdhidCb_t *)prc->pUserData)->ctrl(prc,
                                                USBDHID_CB_REQ_GET_REPORT(0),
                                                (uint8_t *)&s->ptr, s->len);
        }
      }
      result = USBDIF_STATUS_SUCCESS;
      break;
    }
  }

fail:
  return result;
}


static usbdifStatus_t
UsbdHidDataInDone(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdHidHandle_t        *pHid;

  pHid = (usbdHidHandle_t *) prc->pClassData;

  if(pHid != USBDIF_NULL) {
    pHid->TxState = 0;
    return USBDIF_STATUS_SUCCESS;
  } else {
    return USBDIF_STATUS_UNKNOWN;
  }
}
static usbdifStatus_t
UsbdHidDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdHidHandle_t       *pHid;
  int                   val;

  if(size < 0) goto fail;

  pHid = (usbdHidHandle_t *) prc->pClassData;

  /* check to registered the pointer of the hid class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  /* exec callback with rx data */
  pHid->RxLength = size;
  val = pHid->RxLength;
  ((usbdhidCb_t *)prc->pUserData)->recv(prc, pHid->RxBuffer, &val);

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}


int
UsbdHidIsTxBusy(int rcnum)
{
  usbdHidHandle_t       *pHid;
  pHid = (usbdHidHandle_t*) usbdif.rc[rcnum].pClassData;

  return pHid->TxState;
}


usbdifStatus_t
UsbdHidTransmit(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  usbdifClassDef_t      *prc;
  usbdHidHandle_t       *pHid;

  prc = &usbdif.rc[rcnum];
  pHid = (usbdHidHandle_t*) prc->pClassData;


  if(pHid == USBDIF_NULL) goto fail;

  /*RtosSemTake(pHid->idSemSend);*/

  if(!pHid->enOutputReport) goto fail_sem;

  if(pHid->TxState != 0) {
    result = USBDIF_STATUS_UNKNOWN;
    goto fail_sem;
  }

  /* Tx Transfer in progress */
  pHid->TxState = 1;

  /* Transmit this data */
  pHid->TxBuffer = ptr;
  pHid->TxLength = len;
  UsbdevTransmit(prc->dev, pHid->epIn,
                 pHid->TxBuffer, pHid->TxLength);

  result = USBDIF_STATUS_SUCCESS;

fail_sem:
  /*RtosSemRelease(pHid->idSemSend);*/

fail:
  return result;
}


usbdifStatus_t
UsbdHidReceiving(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int           szOut;
  int           speed;

  usbdifClassDef_t *prc;
  usbdHidHandle_t  *pHid;

  prc = &usbdif.rc[rcnum];

  pHid = (usbdHidHandle_t*) prc->pClassData;
  speed = usbdif.sc[prc->dev].speed;

  /* Suspend or Resume USB Out process */
  if(pHid == USBDIF_NULL) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  /* Prepare Out endpoint to receive next packet */
  switch(speed) {
  case  USBIF_SPEED_FULL:
    szOut = USBDHID_PACKET_SIZE_FS; break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szOut = USBDHID_PACKET_SIZE_HS; break;
  }
  if(szOut > len) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }
  pHid->RxBuffer = ptr;
  UsbdevPrepareRecv(usbdif.rc[rcnum].dev, pHid->epOut,
                    pHid->RxBuffer, szOut);

  result =  USBDIF_STATUS_SUCCESS;

fail:
  return result;
}

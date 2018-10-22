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

#define _USBDCDC_C_

#include "usb_def.h"
#include "usbdif.h"
#include "usb_desc.h"

#include "rtos.h"

#include "usbdcdc.h"


extern struct _stUsbdif        usbdif;

usbifClassCb_t  usbdCdcClassCb = {
  UsbdCdcInit,
  UsbdCdcDeInit,
  UsbdCdcSetup,
  UsbdCdcDataIn,
  UsbdCdcDataOut,
  USBDIF_NULL,
};


static usbdifStatus_t
UsbdCdcInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = USBDIF_STATUS_UNKNOWN;

  usbdCdcHandle_t               *pCdc;
  int                           szIn, szOut;
  int                           speed;

  speed = usbdif.sc[prc->dev].speed;

  switch(speed) {
  case  USBIF_SPEED_FULL:
    szIn  = USBDCDC_PACKET_SIZE_FS_IN;
    szOut = USBDCDC_PACKET_SIZE_FS_OUT;
    break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szIn  = USBDCDC_PACKET_SIZE_HS_IN;
    szOut = USBDCDC_PACKET_SIZE_HS_OUT;
  }

  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdCdcHandle_t));
    if(prc->pClassData == USBDIF_NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(sizeof (usbdCdcHandle_t)));
  }

  pCdc = (usbdCdcHandle_t*) prc->pClassData;

  pCdc->epIn   = USBDESC_EP_CDC_IN;
  pCdc->epOut  = USBDESC_EP_CDC_OUT;
  pCdc->epCtrl = USBDESC_EP_CDC_CTRL;

  UsbdevOpenEp(prc->dev, pCdc->epIn,   USBIF_EP_BULK, szIn);
  UsbdevOpenEp(prc->dev, pCdc->epOut,  USBIF_EP_BULK, szOut);
  UsbdevOpenEp(prc->dev, pCdc->epCtrl, USBIF_EP_INTR, USBDCDC_PACKET_SIZE_CMD);

  /* Init Xfer states */
  pCdc->TxState = 0;
  pCdc->RxState = 0;

  /* create the mutex for send */
  /*pCdc->idSemSend = RtosSemCreateMutex();*/

  /* Init  physical Interface components */
  if(prc->pUserData) ((usbdcdcCb_t *)prc->pUserData)->init(prc);

#if 1
  /* Prepare Out endpoint to receive next packet */
  DevUsbPrepareReceive(prc->dev, USBDESC_EP_CDC_OUT, pCdc->RxBuffer, szOut);
#endif

  ret = USBDIF_STATUS_SUCCESS;

fail:
  return ret;
}


static usbdifStatus_t
UsbdCdcDeInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = 0;
  usbdCdcHandle_t               *pCdc;

  pCdc = (usbdCdcHandle_t *)prc->pClassData;

  /* Close all EPs */
  UsbdevCloseEp(prc->dev, pCdc->epIn);
  UsbdevCloseEp(prc->dev, pCdc->epOut);
  UsbdevCloseEp(prc->dev, pCdc->epCtrl);

  /* DeInit  physical Interface components */
  if(prc->pClassData != USBDIF_NULL) {
    ((usbdcdcCb_t *)prc->pUserData)->deinit(prc);
    UsbdifFree(prc->pClassData);
    prc->pClassData = USBDIF_NULL;
  }

  return ret;
}


static usbdifStatus_t
UsbdCdcSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  usbdCdcHandle_t       *pCdc = (usbdCdcHandle_t*) prc->pClassData;
  static uint8_t        ifalt = 0;
  int                   unit;

  unit = prc->dev;
  if((s->bmRequest & USB_BMREQ_TYPE_MASK) == USB_BMREQ_TYPE_CLASS) {
    switch(s->bRequest) {
    case CDC_SET_LINE_CODING:
      ((usbdcdcCb_t *)prc->pUserData)->ctrl(prc, s->bRequest,
                                            (uint8_t *)s->buf,
                                            s->wLength);
      break;
    case CDC_GET_LINE_CODING:
      ((usbdcdcCb_t *)prc->pUserData)->ctrl(prc, pCdc->CmdOpCode,
                                            (uint8_t *) s->buf,
                                            s->wLength);
      /*UsbdevCtrlSendData(unit, (uint8_t *)pCdc->data, s->wLength);*/
      break;
    case CDC_SET_CONTROL_LINE_STATE:
      ((usbdcdcCb_t *)prc->pUserData)->ctrl(prc, s->bRequest,
                                            (uint8_t*)s,
                                            0);
      break;

    default:
      re = USBDIF_STATUS_STALL;
      break;
    }
  } else {
    re = USBDIF_STATUS_STALL;
  }

  return re;
}

static usbdifStatus_t
UsbdCdcDataIn(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdCdcHandle_t        *pCdc;

  pCdc = (usbdCdcHandle_t*) prc->pClassData;

  if(pCdc != USBDIF_NULL) {
    pCdc->TxState = 0;
    return USBDIF_STATUS_SUCCESS;
  } else {
    return USBDIF_STATUS_UNKNOWN;
  }
}
static usbdifStatus_t
UsbdCdcDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdCdcHandle_t       *pCdc;
  int                   val;

  if(size < 0) goto fail;

  pCdc = (usbdCdcHandle_t*) prc->pClassData;

  /* check to registered the pointer of the cdc class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  /* exec callback with rx data */
  pCdc->RxLength = size;
  ((usbdcdcCb_t *)prc->pUserData)->recv(prc, pCdc->RxBuffer, pCdc->RxLength);
  printf("usbdcdc recv %x %x\r\n", *pCdc->RxBuffer, pCdc->RxLength);

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}



int
UsbdCdcIsTxBusy(int rcnum)
{
  usbdCdcHandle_t       *pCdc;
  pCdc = (usbdCdcHandle_t*) usbdif.rc[rcnum].pClassData;

  return pCdc->TxState;
}


usbdifStatus_t
UsbdCdcTransmit(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  usbdifClassDef_t      *prc;
  usbdCdcHandle_t       *pCdc;

  prc = &usbdif.rc[rcnum];
  pCdc = (usbdCdcHandle_t*) prc->pClassData;


  if(pCdc == USBDIF_NULL) goto fail;

  /*RtosSemTake(pCdc->idSemSend);*/

  if(pCdc->TxState != 0) {
    result = USBDIF_STATUS_UNKNOWN;
    goto fail_sem;
  }

  /* Tx Transfer in progress */
  pCdc->TxState = 1;

  /* Transmit this data */
  pCdc->TxBuffer = ptr;
  pCdc->TxLength = len;
  UsbdevTransmit(prc->dev, pCdc->epIn,
                 pCdc->TxBuffer, pCdc->TxLength);

  result = USBDIF_STATUS_SUCCESS;

fail_sem:
  /*RtosSemRelease(pCdc->idSemSend);*/

fail:
  return result;
}


usbdifStatus_t
UsbdCdcReceiving(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int           szOut;
  int           speed;

  usbdifClassDef_t *prc;
  usbdCdcHandle_t  *pCdc;

  prc = &usbdif.rc[rcnum];

  pCdc = (usbdCdcHandle_t*) prc->pClassData;
  speed = usbdif.sc[prc->dev].speed;

  /* Suspend or Resume USB Out process */
  if(pCdc == USBDIF_NULL) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  /* Prepare Out endpoint to receive next packet */
  switch(speed) {
  case  USBIF_SPEED_FULL:
    szOut = USBDCDC_PACKET_SIZE_FS_OUT; break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szOut = USBDCDC_PACKET_SIZE_HS_OUT; break;
  }
  if(szOut > len) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }
  pCdc->RxBuffer = ptr;
  UsbdevPrepareRecv(usbdif.rc[rcnum].dev, pCdc->epOut,
                    pCdc->RxBuffer, szOut);

  result =  USBDIF_STATUS_SUCCESS;

fail:
  return result;
}

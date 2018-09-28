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

#define _USBDCDCRNDIS_C_

#include "usb_def.h"
#include "usbdif.h"
#include "usb_desc.h"

#include "rtos.h"

#include "usbdcdcrndis.h"


#if 0
#include "system.h"
#include "gpio.h"
#endif

extern struct _stUsbdif        usbdif;

usbifClassCb_t  usbdCdcRndisClassCb = {
  UsbdCdcRndisInit,
  UsbdCdcRndisDeInit,
  UsbdCdcRndisSetup,
  UsbdCdcRndisDataIn,
  UsbdCdcRndisDataOut,
  USBDIF_NULL,
};


static usbdifStatus_t
UsbdCdcRndisInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = USBDIF_STATUS_UNKNOWN;

  usbdCdcRndisHandle_t          *pCdc;
  int                           szIn, szOut;
  int                           speed;

  speed = usbdif.sc[prc->dev].speed;

  switch(speed) {
  case  USBIF_SPEED_FULL:
    szIn  = USBDCDCRNDIS_PACKET_SIZE_FS_IN;
    szOut = USBDCDCRNDIS_PACKET_SIZE_FS_OUT;
    break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szIn  = USBDCDCRNDIS_PACKET_SIZE_HS_IN;
    szOut = USBDCDCRNDIS_PACKET_SIZE_HS_OUT;
  }

  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdCdcRndisHandle_t));
    if(prc->pClassData == USBDIF_NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(sizeof (usbdCdcRndisHandle_t)));
  }

  pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;

  pCdc->epIn   = USBDESC_EP_CDCRNDIS_IN;
  pCdc->epOut  = USBDESC_EP_CDCRNDIS_OUT;
  pCdc->epCtrl = USBDESC_EP_CDCRNDIS_CTRL;

  UsbdevOpenEp(prc->dev, pCdc->epIn,   USBIF_EP_BULK, szIn);
  UsbdevOpenEp(prc->dev, pCdc->epOut,  USBIF_EP_BULK, szOut);
  UsbdevOpenEp(prc->dev, pCdc->epCtrl, USBIF_EP_INTR, USBDCDCRNDIS_PACKET_SIZE_CMD);

  /* Init Xfer states */
  pCdc->TxState = 0;
  pCdc->RxState = 0;

  /* create the mutex for send */
  /*pCdc->idSemSend = RtosSemCreateMutex();*/

  /* Init  physical Interface components */
  if(prc->pUserData && ((usbdcdcRndisCb_t *)prc->pUserData)) {
    ((usbdcdcRndisCb_t *)prc->pUserData)->init(prc);
  }

#if 0
  /* Prepare Out endpoint to receive next packet */
  UsbdPrepareRecv(prc->dev, USBDESC_EP_CDCRNDIS_OUT, pCdc->RxBuffer, szOut);
#endif

  ret = USBDIF_STATUS_SUCCESS;

fail:
  return ret;
}


static usbdifStatus_t
UsbdCdcRndisDeInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = 0;
  usbdCdcRndisHandle_t          *pCdc;

  pCdc = (usbdCdcRndisHandle_t *)prc->pClassData;

  /* Close all EPs */
  UsbdevCloseEp(prc->dev, pCdc->epIn);
  UsbdevCloseEp(prc->dev, pCdc->epOut);
  UsbdevCloseEp(prc->dev, pCdc->epCtrl);

  /* DeInit  physical Interface components */
  if(prc->pClassData != USBDIF_NULL) {
    ((usbdcdcRndisCb_t *)prc->pUserData)->deinit(prc);
    UsbdifFree(prc->pClassData);
    prc->pClassData = USBDIF_NULL;
  }

  return ret;
}


static usbdifStatus_t
UsbdCdcRndisSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  usbdCdcRndisHandle_t   *pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;
  static uint8_t ifalt = 0;
  int                   dev;

  printf("XXX %x %x %x\r\n", s->bmRequest, s->bRequest, s->wIndex);

  dev = prc->dev;
  switch(s->bmRequest & USB_BMREQ_TYPE_MASK) {
  case USB_BMREQ_TYPE_CLASS:
    if(s->wLength) {
      if(s->bmRequest & 0x80) {
        ((usbdcdcRndisCb_t *)prc->pUserData)->ctrl(prc, s->bRequest,
                                                   (uint8_t *)s->buf,
                                                   s->wLength);
        UsbdevCtrlSendData(dev, (uint8_t *)pCdc->data, s->wLength);
      } else {
        if(prc->pUserData && ((usbdcdcRndisCb_t *)prc->pUserData)->ctrl) {
          ((usbdcdcRndisCb_t *)prc->pUserData)->ctrl(prc, pCdc->CmdOpCode,
                                                   (uint8_t *) s->buf,
                                                   s->wLength);
        }

      }

    } else {
      if(prc->pUserData && ((usbdcdcRndisCb_t *)prc->pUserData)->ctrl) {
        ((usbdcdcRndisCb_t *)prc->pUserData)->ctrl(prc, s->bRequest,
                                                 (uint8_t*)s,
                                                 0);
      }
    }
    break;

  default:
    break;
  }

  return USBDIF_STATUS_SUCCESS;
}

static usbdifStatus_t
UsbdCdcRndisDataIn(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdCdcRndisHandle_t          *pCdc;

  pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;

  if(pCdc != USBDIF_NULL) {
    pCdc->TxState = 0;
    return USBDIF_STATUS_SUCCESS;
  } else {
    return USBDIF_STATUS_UNKNOWN;
  }
}
static usbdifStatus_t
UsbdCdcRndisDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdCdcRndisHandle_t       *pCdc;
  int                   val;

  if(size < 0) goto fail;

  pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;

  /* check to registered the pointer of the cdc class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  /* exec callback with rx data */
  pCdc->RxLength = size;
  if(prc->pUserData && ((usbdcdcRndisCb_t *)prc->pUserData)->recv) {
    ((usbdcdcRndisCb_t *)prc->pUserData)->recv(prc, pCdc->RxBuffer, pCdc->RxLength);
  }

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}



int
UsbdCdcRndisIsTxBusy(int rcnum)
{
  usbdCdcRndisHandle_t       *pCdc;
  pCdc = (usbdCdcRndisHandle_t*) usbdif.rc[rcnum].pClassData;

  return pCdc->TxState;
}


usbdifStatus_t
UsbdCdcRndisTransmit(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  usbdifClassDef_t      *prc;
  usbdCdcRndisHandle_t  *pCdc;

  prc = &usbdif.rc[rcnum];
  pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;


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
UsbdCdcRndisReceiving(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int           szOut;
  int           speed;

  usbdifClassDef_t      *prc;
  usbdCdcRndisHandle_t  *pCdc;

  prc = &usbdif.rc[rcnum];

  pCdc = (usbdCdcRndisHandle_t*) prc->pClassData;
  speed = usbdif.sc[prc->dev].speed;

  /* Suspend or Resume USB Out process */
  if(pCdc == USBDIF_NULL) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  /* Prepare Out endpoint to receive next packet */
  switch(speed) {
  case  USBIF_SPEED_FULL:
    szOut = USBDCDCRNDIS_PACKET_SIZE_FS_OUT; break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szOut = USBDCDCRNDIS_PACKET_SIZE_HS_OUT; break;
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

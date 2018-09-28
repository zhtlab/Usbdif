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

#define _USBDMSC_C_

#include        "usb_def.h"
#include        "usbdif.h"
#include        "usb_desc.h"

#include        "rtos.h"

#include        "usbdmsc_bot.h"
#include        "usbdmsc.h"


extern struct _stUsbdif        usbdif;

usbifClassCb_t  usbdMscClassCb = {
  UsbdMscInit,
  UsbdMscDeInit,
  UsbdMscSetup,
  UsbdMscDataInDone,
  UsbdMscDataOut,
  USBDIF_NULL,
};
uint8_t         usbdmscBufRx[64];


static usbdifStatus_t
UsbdMscInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = USBDIF_STATUS_UNKNOWN;

  usbdMscHandle_t               *pMsc;
  int                           szIn, szOut;
  int                           speed;

  speed = usbdif.sc[prc->dev].speed;

  switch(speed) {
  case  USBIF_SPEED_FULL:
    szIn  = USBDMSC_PACKET_SIZE_FS_IN;
    szOut = USBDMSC_PACKET_SIZE_FS_OUT;
    break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szIn  = USBDMSC_PACKET_SIZE_HS_IN;
    szOut = USBDMSC_PACKET_SIZE_HS_OUT;
  }

  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdMscHandle_t));
    if(prc->pClassData == USBDIF_NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(sizeof (usbdMscHandle_t)));
  }

  pMsc = (usbdMscHandle_t*) prc->pClassData;

  pMsc->epIn   = USBDESC_EP_MSC_IN;
  pMsc->epOut  = USBDESC_EP_MSC_OUT;

  UsbdevOpenEp(prc->dev, pMsc->epIn,   USBIF_EP_BULK, szIn);
  UsbdevOpenEp(prc->dev, pMsc->epOut,  USBIF_EP_BULK, szOut);

  /* Init Xfer states */
  pMsc->TxState = 0;
  pMsc->RxState = 0;

  if(prc->pUserData) ((usbdmscCb_t *)prc->pUserData)->init(prc);        /* adhoc need?? */
  UsbdmscBotInit(prc);

  /* Prepare Out endpoint to receive next packet */
  pMsc->RxBuffer = usbdmscBufRx;
  pMsc->RxLength = sizeof(usbdmscBufRx);
  UsbdevPrepareRecv(prc->dev, USBDESC_EP_MSC_OUT, pMsc->RxBuffer, pMsc->RxLength);

  ret = USBDIF_STATUS_SUCCESS;

fail:
  return ret;
}


static usbdifStatus_t
UsbdMscDeInit(usbdifClassDef_t *prc, uint8_t cfgidx)
{
  uint8_t ret = 0;
  usbdMscHandle_t               *pMsc;

  pMsc = (usbdMscHandle_t *)prc->pClassData;

  UsbdmscBotDeinit(prc);

  /* Close all EPs */
  UsbdevCloseEp(prc->dev, pMsc->epIn);
  UsbdevCloseEp(prc->dev, pMsc->epOut);

  /* DeInit  physical Interface components */
  if(prc->pClassData != USBDIF_NULL) {  /* adhoc need???? */
    UsbdifFree(prc->pClassData);
    prc->pClassData = USBDIF_NULL;
  }

  return ret;
}


static usbdifStatus_t
UsbdMscSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  UsbdMscBotSetup(prc, s);

  return USBDIF_STATUS_SUCCESS;
}

static usbdifStatus_t
UsbdMscDataInDone(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  UsbdmscBotDataInDone(prc, epnum);

  return result;
}
static usbdifStatus_t
UsbdMscDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  usbdifStatus_t        re;
  usbdMscHandle_t       *pMsc;
  int                   val;

  if(size < 0) goto fail;
  pMsc = (usbdMscHandle_t*) prc->pClassData;

  /* check to registered the pointer of the msc class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  /* exec callback with rx data */
  pMsc->RxLength = size;
  UsbdmscBotDataOut(prc, epnum, &pMsc->RxBuffer, (int *)&pMsc->RxLength);
  result = USBDIF_STATUS_SUCCESS;

fail:
  if(!pMsc->RxBuffer || !pMsc->RxLength || !size) {
    pMsc->RxBuffer = usbdmscBufRx;
    pMsc->RxLength = sizeof(usbdmscBufRx);
  }
  UsbdevPrepareRecv(prc->dev, USBDESC_EP_MSC_OUT, pMsc->RxBuffer, pMsc->RxLength);

  return result;
}

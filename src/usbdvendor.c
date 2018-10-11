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

#define _USBDVENDOR_C_

#include "usb_def.h"
#include "usbdif.h"
#include "usb_desc.h"

#include "rtos.h"

#include "usbdvendor.h"


extern struct _stUsbdif        usbdif;

usbifClassCb_t  usbdVendorClassCb = {
  UsbdVendorInit,
  UsbdVendorDeInit,
  UsbdVendorSetup,
  USBDIF_NULL, /*UsbdVendorDataIn,*/
  USBDIF_NULL, /*UsbdVendorDataOut,*/
  USBDIF_NULL,
};


/**
  * @brief  initialize
  * @details initilize for vendor transfer
  * @param  prc    descriptor
  * @param  speed  speed
  * @retval result
  */
static usbdifStatus_t
UsbdVendorInit(usbdifClassDef_t *prc, uint8_t speed)
{
  uint8_t ret = USBDIF_STATUS_UNKNOWN;

  usbdVendorHandle_t               *pVendor;
  int                           szIn, szOut;

  //speed = usbdif.sc[prc->dev].speed;

  switch(speed) {
  case  USBIF_SPEED_FULL:
    szIn  = USBDVENDOR_PACKET_SIZE_FS_IN;
    szOut = USBDVENDOR_PACKET_SIZE_FS_OUT;
    break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szIn  = USBDVENDOR_PACKET_SIZE_HS_IN;
    szOut = USBDVENDOR_PACKET_SIZE_HS_OUT;
  }

  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdVendorHandle_t));
    if(prc->pClassData == USBDIF_NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(sizeof (usbdVendorHandle_t)));
  }

  pVendor = (usbdVendorHandle_t*) prc->pClassData;

  pVendor->epIn   = USBDESC_EP_VENDOR_IN;

  UsbdevOpenEp(prc->dev, pVendor->epIn,   USBIF_EP_BULK, szIn);

  /* Init Xfer states */
  pVendor->TxState = 0;
  pVendor->RxState = 0;

  /* create the mutex for send */
  /*pVendor->idSemSend = RtosSemCreateMutex();*/

  /* Init  physical Interface components */
  if(prc->pUserData) ((usbdvendorCb_t *)prc->pUserData)->init(prc);

  ret = USBDIF_STATUS_SUCCESS;

fail:
  return ret;
}


/**
  * @brief  de-initialize
  * @details de-initilize for vendor transfer
  * @param  prc    descriptor
  * @param  speed  speed
  * @retval result
  */
static usbdifStatus_t
UsbdVendorDeInit(usbdifClassDef_t *prc, uint8_t speed)
{
  uint8_t ret = 0;
  usbdVendorHandle_t               *pVendor;

  pVendor = (usbdVendorHandle_t *)prc->pClassData;

  /* Close all EPs */
  UsbdevCloseEp(prc->dev, pVendor->epIn);

  /* DeInit  physical Interface components */
  if(prc->pClassData != USBDIF_NULL) {
    ((usbdvendorCb_t *)prc->pUserData)->deinit(prc);
    UsbdifFree(prc->pClassData);
    prc->pClassData = USBDIF_NULL;
  }

  return ret;
}


/**
  * @brief   setup
  * @details setup vendor transfer
  * @param  prc    descriptor
  * @param  s      pointer of setup structure
  * @retval result
  */
static usbdifStatus_t
UsbdVendorSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  int                   re = USBDIF_STATUS_UNKNOWN;

  //UsbifShowSetup(s);
  if(prc->pUserData) re = ((usbdvendorCb_t *)prc->pUserData)->ctrl(prc, s);

  return re;
}




#if 0

/**
  * @brief   DataIn done
  * @details data send complite to host
  * @param  prc    descriptor
  * @param  epnum  endpoint number
  * @retval result
  */
static usbdifStatus_t
UsbdVendorDataIn(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdVendorHandle_t        *pVendor;

  pVendor = (usbdVendorHandle_t*) prc->pClassData;

  if(pVendor != USBDIF_NULL) {
    pVendor->TxState = 0;
    return USBDIF_STATUS_SUCCESS;
  } else {
    return USBDIF_STATUS_UNKNOWN;
  }
}
/**
  * @brief   DataOut
  * @details data send was come
  * @param  prc    descriptor
  * @param  epnum  endpoint number
  * @param  size   received data size
  * @retval result
  */
static usbdifStatus_t
UsbdVendorDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdVendorHandle_t       *pVendor;
  int                   val;

  if(size < 0) goto fail;

  pVendor = (usbdVendorHandle_t*) prc->pClassData;

  /* check to registered the pointer of the vendor class data */
  if(prc->pClassData == USBDIF_NULL) goto fail;

  /* exec callback with rx data */
  pVendor->RxLength = size;
  ((usbdvendorCb_t *)prc->pUserData)->recv(prc, pVendor->RxBuffer, pVendor->RxLength);
  printf("usbdvendor recv %x %x\r\n", *pVendor->RxBuffer, pVendor->RxLength);

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}
#endif


#if 0
/**
  * @brief  Is tx busy
  * @details check data buffer full
  * @param  rcnum  registered class number
  * @retval result
  */
int
UsbdVendorIsTxBusy(int rcnum)
{
  usbdVendorHandle_t       *pVendor;
  pVendor = (usbdVendorHandle_t*) usbdif.rc[rcnum].pClassData;

  return pVendor->TxState;
}


/**
  * @brief  transmit
  * @details transmit data to host (user can call this)
  * @param  rcnum  registered class number
  * @param  ptr    pointer of send data
  * @param  len    data length to send
  * @retval result
  */
usbdifStatus_t
UsbdVendorTransmit(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  usbdifClassDef_t      *prc;
  usbdVendorHandle_t       *pVendor;

  prc = &usbdif.rc[rcnum];
  pVendor = (usbdVendorHandle_t*) prc->pClassData;


  if(pVendor == USBDIF_NULL) goto fail;

  /*RtosSemTake(pVendor->idSemSend);*/

  if(pVendor->TxState != 0) {
    result = USBDIF_STATUS_UNKNOWN;
    goto fail_sem;
  }

  /* Tx Transfer in progress */
  pVendor->TxState = 1;

  /* Transmit this data */
  pVendor->TxBuffer = ptr;
  pVendor->TxLength = len;
  UsbdevTransmit(prc->dev, pVendor->epIn,
                 pVendor->TxBuffer, pVendor->TxLength);

  result = USBDIF_STATUS_SUCCESS;

fail_sem:
  /*RtosSemRelease(pVendor->idSemSend);*/

fail:
  return result;
}


/**
  * @brief  prepare receiving
  * @details prepare data receiving
  * @param  rcnum  registered class number
  * @param  ptr    pointer of buffer for to store
  * @param  len    data length to store
  * @retval result
  */
usbdifStatus_t
UsbdVendorReceiving(int rcnum, uint8_t *ptr, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int           szOut;
  int           speed;

  usbdifClassDef_t *prc;
  usbdVendorHandle_t  *pVendor;

  prc = &usbdif.rc[rcnum];

  pVendor = (usbdVendorHandle_t*) prc->pClassData;
  speed = usbdif.sc[prc->dev].speed;

  /* Suspend or Resume USB Out process */
  if(pVendor == USBDIF_NULL) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  /* Prepare Out endpoint to receive next packet */
  switch(speed) {
  case  USBIF_SPEED_FULL:
    szOut = USBDVENDOR_PACKET_SIZE_FS_OUT; break;
  default:
    /*case  USBIF_SPEED_HIGH:*/
    szOut = USBDVENDOR_PACKET_SIZE_HS_OUT; break;
  }
  if(szOut > len) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }
  pVendor->RxBuffer = ptr;
  UsbdevPrepareRecv(usbdif.rc[rcnum].dev, pVendor->epOut,
                    pVendor->RxBuffer, szOut);

  result =  USBDIF_STATUS_SUCCESS;

fail:
  return result;
}
#endif

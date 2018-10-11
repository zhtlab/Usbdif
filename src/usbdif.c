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

#define _USBDIF_C_

#include        <stdlib.h>
#include        <stdio.h>
#include        "config.h"

#include        "usb_def.h"
#include        "usb_desc.h"

#include        "usbdif.h"
#include        "usb_audio.h"


struct _stUsbdif        usbdif;
extern usbifClassCb_t           usbdAudioClassCb;
extern usbifClassCb_t           usbdCdcClassCb;
extern usbifClassCb_t           usbdMscClassCb;
extern usbifClassCb_t           usbdCdcRndisClassCb;
#ifdef USBDESC_ENABLE_VENDOR
extern usbifClassCb_t           usbdVendorClassCb;
#endif

usbifClassCb_t  *usbifClassCbTbl[] = {
  NULL,                 /*  0 none */
  &usbdAudioClassCb,    /*  1 AUDIO */
  &usbdCdcClassCb,      /*  2 CDC */
  NULL,                 /*  3 HID */
  NULL,                 /*  4 none */
  NULL,                 /*  5 PHYSICAL */
  NULL,                 /*  6 IMAGE */
  NULL,                 /*  7 PRINTER */
  &usbdMscClassCb,      /*  8 MASS STORAGE */
  NULL,                 /*  9 HUB */
  &usbdCdcRndisClassCb, /*  A CDC-DATA */
  NULL,                 /*  B CHIP/SMART card */
  NULL,                 /*  C none */
  NULL,                 /*  D CONTENT SECURITY */
  NULL,                 /*  E VIDEO */
  NULL,                 /*  F Personal healthcare */

  NULL,                 /* 10 n/a */
  NULL,                 /* 11 n/a */
  NULL,                 /* 12 n/a */
#ifdef USBDESC_ENABLE_VENDOR
  &usbdVendorClassCb,   /* 13 Vendor unique   aliased by 0xff */
#else
  NULL,
#endif
};
#define USBIF_CLASS_CB_TBL_COUNT     (sizeof(usbifClassCbTbl)/sizeof(usbifClassCb_t))


/**
  * @brief  init event entry in USBDIF
  * @details this funcion is called at initialize
  * @param  dev the descriptor number of the modeule device
  * @param  speed  FS, HS, SS
  * @retval result
  */
uint8_t
UsbifCbInit(int dev, int speed)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  struct _stUsbdifDev   *psc;
  usbdifClassDef_t      *prc;

  int                   i, num, type;
  usbifClassCb_t        *cb;

  psc = &usbdif.sc[dev];
  for(i = 0; i < psc->classnum; i++) {
    num = psc->rcnumByClassnum[i];
    if(num != USBDIF_CLASSPOS_INVALID) {
      prc = &usbdif.rc[num];
      cb = usbifClassCbTbl[prc->type];
      if(cb && cb->init) cb->init(prc, speed);
    }
  }

  return re;
}
/**
  * @brief  deinit event entry in USBDIF
  * @details this funcion is called at deinitialize
  * @param  dev the descriptor number of the modeule device
  * @param  speed  FS, HS, SS
  * @retval result
  */
uint8_t
UsbifCbDeInit(int dev, int speed)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  struct _stUsbdifDev   *psc;
  usbdifClassDef_t      *prc;
  int                   i, num, type;
  usbifClassCb_t        *cb;

  psc = &usbdif.sc[dev];
  for(i = 0; i < USBIF_CLASS_CB_TBL_COUNT; i++) {
    num = psc->rcnumByClassnum[i];
    if(num != USBDIF_CLASSPOS_INVALID) {
      prc = &usbdif.rc[num];
      printf("XXXX type %x\r\n", prc->type);
      cb = usbifClassCbTbl[prc->type];
      if(cb && cb->deinit) cb->deinit(prc, speed);
    }
  }

  return re;
}


/**
  * @brief  setup event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the SETUP packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  s   the pointer of setup structure
  * @retval result
  */
uint8_t
UsbifCbSetup(int dev, usbifSetup_t *s)
{
  usbdifStatus_t        re = USBDIF_STATUS_UNKNOWN;
  int                   num, index, classtype;
  struct _stUsbdifDev   *psc;
  int                   i;
  usbifClassCb_t        *cb;
  uint16_t              len;
  uint8_t               *pbuf;
  usbdifClassDef_t      *prc;

  psc = &usbdif.sc[dev];
  index = s->wIndex & 0x7f;
  if(s->wIndex & 0x80) {
    index += USBDIF_RCNUM_OFFSET_IN;
  }
  classtype = 0;

#if 0
  UsbifShowSetup(s);
#endif

  switch (s->bmRequest & USB_BMREQ_RECIPIENT_MASK) {
  case USB_BMREQ_RECIPIENT_DEVICE:
    /* call the first entry of classnum table */
    num = psc->rcnumByClassnum[0];
    if(num != USBDIF_CLASSPOS_INVALID) {
      prc = &usbdif.rc[num];
      classtype = prc->type;
    }
    break;
  case USB_BMREQ_RECIPIENT_INTERFACE:
    num = psc->rcnumByIfnum[index];
    if(num != USBDIF_CLASSPOS_INVALID) {
      prc = &usbdif.rc[num];
      classtype = prc->type;
    }
    break;

  case USB_BMREQ_RECIPIENT_ENDPOINT:
    num = psc->rcnumByEpnum[index];
    if(num != USBDIF_CLASSPOS_INVALID) {
      prc = &usbdif.rc[num];
      classtype = prc->type;
    }
    break;
  default:
    break;
  }

  //printf("## usbdif classtype %x\r\n", classtype);

  if(classtype) {
    cb = usbifClassCbTbl[classtype];
    if(cb && cb->setup) re = cb->setup(prc, s);

  } else if(classtype == 0) {
    switch(s->bmRequest & USB_BMREQ_TYPE_MASK) {
    case USB_BMREQ_TYPE_CLASS:
      UsbifShowSetup(s);
      printf("  this class is not registered yet\r\n");
      break;

    case USB_BMREQ_TYPE_VENDOR:
      prc = &usbdif.rc[0];                      /* adhoc */
      cb = usbifClassCbTbl[prc->type];
      if(cb && cb->setup) re = cb->setup(prc, s);
      break;

    case USB_BMREQ_TYPE_STANDARD:
      printf("XXXX standard \r\n");
      break;
    }
  } else {
  }

  return re;
}


/**
  * @brief  data out event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the DATA OUT packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  epnum endpoint number
  * @retval result
  */
uint8_t
UsbifCbDataOut(int dev, uint8_t epnum, int size)
{
  usbdifStatus_t        re = USBDIF_STATUS_UNKNOWN;
  int                   num;
  struct _stUsbdifDev   *psc;
  usbifClassCb_t        *cb;
  usbdifClassDef_t      *prc;

  epnum &= 0x7f;
  if(epnum >= USBDIF_MAX_EPNUM) {
    re = USBDIF_STATUS_OUTOFRANGE;
    goto fail;
  }

  psc = &usbdif.sc[dev];
  num = psc->rcnumByEpnum[epnum];
  if(num != USBDIF_CLASSPOS_INVALID) {
    prc = &usbdif.rc[num];
    cb = usbifClassCbTbl[prc->type];
    if(cb && cb->dataOut) re = cb->dataOut(prc, epnum, size);
  }

fail:
  return re;
}
/**
  * @brief  data in event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the DATA IN packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  epnum endpoint number (don't care set the DIR bit, stripped in this function)
  * @retval result
  */
uint8_t
UsbifCbDataInDone(int dev, uint8_t epnum)
{
  usbdifStatus_t        re = USBDIF_STATUS_UNKNOWN;
  int                   num;
  struct _stUsbdifDev   *psc;
  usbifClassCb_t        *cb;
  usbdifClassDef_t      *prc;

  epnum &= 0x7f;
  if(epnum >= USBDIF_MAX_EPNUM) {
    re = USBDIF_STATUS_OUTOFRANGE;
    goto fail;
  }

  psc = &usbdif.sc[dev];
  num = psc->rcnumByEpnum[epnum + USBDIF_RCNUM_OFFSET_IN];
  if(num != USBDIF_CLASSPOS_INVALID) {
    prc = &usbdif.rc[num];
    cb = usbifClassCbTbl[prc->type];
    if(cb && cb->dataInDone) re = cb->dataInDone(prc, epnum);
  }

fail:
  return re;
}


/**
  * @brief  data in event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the DATA IN packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  sof the pointer of the sof structure
  * @retval result
  */
uint8_t
UsbifCbSof(int dev, usbifSof_t *sof)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  int                   i;
  int                   num;
  struct _stUsbdifDev   *psc;
  usbifClassCb_t        *cb;

  psc = &usbdif.sc[dev];
  for(i = 0; i < psc->classnum; i++) {
    cb = usbifClassCbTbl[i];
    if(cb && cb->sof) cb->sof(&usbdif.rc[i], sof);
  }

  return re;
}


uint8_t
UsbifCbGetConfigDesc(int dev, uint8_t **ppDesc, uint16_t *pLen, int speed)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int                   num;
  struct _stUsbdifDev   *psc;
  usbifClassCb_t        *cb;

  if(dev < 0 || dev >= USBDIF_MAX_DEVICE) goto fail;

  psc = &usbdif.sc[dev];
  *pLen   = psc->initParam.lenConfigDesc;
  *ppDesc = psc->initParam.pConfigDesc;
  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}


uint8_t
UsbifCbGetQualifierDesc(int dev, uint8_t **ppDesc, uint16_t *pLen, int speed)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  int                   num;
  struct _stUsbdifDev   *psc;
  usbifClassCb_t        *cb;

  if(dev < 0 || dev >= USBDIF_MAX_DEVICE) goto fail;

  psc = &usbdif.sc[dev];
  *pLen   = psc->initParam.lenDeviceQualifierDesc;
  *ppDesc = psc->initParam.pDeviceQualifierDesc;
  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}


usbdifStatus_t
UsbifInit(int unit, usbdifInitParam_t *pUsbInit)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  uint8_t               id;
  struct _stUsbdifDev   *psc;

  if(unit < 0) {                 /* all clear */
    memset(&usbdif, 0, sizeof(usbdif));
    goto end;
  }

  if(unit >= USBDIF_MAX_DEVICE) goto fail;
  if(!pUsbInit) goto fail;

  psc = &usbdif.sc[unit];

  memset(psc, 0, sizeof(usbdif.sc[0]));

  memset(psc->rcnumByClassnum, USBDIF_CLASSPOS_INVALID,
         sizeof(psc->rcnumByClassnum));
  memset(psc->rcnumByIfnum, USBDIF_CLASSPOS_INVALID,
         sizeof(psc->rcnumByIfnum));
  memset(psc->rcnumByEpnum, USBDIF_CLASSPOS_INVALID,
         sizeof(psc->rcnumByEpnum));


  /* initialize the usb device
   * register the all descriptors (device, config, string, etc)
   */
  psc->initParam = *pUsbInit;   /* delete */
  extern const usbdifDescritprTbl_t usbDescritptorTbl;  /* adhoc */
  psc->pDescTbl = &usbDescritptorTbl;

  result = UsbdevInit(unit, &psc->initParam);

  result = USBDIF_STATUS_SUCCESS;
fail:
end:
  return result;
}


int
UsbifRegisterClass(int dev, const usbdifClassDef_t *pClass)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  struct _stUsbdifDev   *psc;
  usbdifClassDef_t      *pc;

  int                   shift;
  int                   i, num;

#if 0
  if(pClass->numClass >= 2 ||
     pClass->type == USBDIF_CLASSTYPE_MASS_STORAGE) goto fail;
#endif


  if(dev < 0 || dev >= USBDIF_MAX_DEVICE) goto fail;
  if(usbdif.rcnum >= USBDIF_MAX_REGCLASS) goto fail;

  psc = &usbdif.sc[dev];
  /* register the rcnumByClassnum list */
  if(psc->rcnumByClassnum[pClass->numClass] != USBDIF_CLASSPOS_INVALID) goto fail;
  num = usbdif.rcnum;
  usbdif.rc[num] = *pClass;     /* copy class information */
  usbdif.rc[num].dev = dev;

  psc->rcnumByClassnum[pClass->numClass] = num;
  if(psc->classnum < pClass->numClass+1) psc->classnum = pClass->numClass+1;

  /* register the rcnumByIfnum list */
  shift = pClass->ifnum;
  for(i = 0; i < USBDIF_RCNUM_TOTAL; i++) {
    if(shift & 1) {
      if(psc->rcnumByIfnum[i] != USBDIF_CLASSPOS_INVALID) goto fail;
      psc->rcnumByIfnum[i] = num;
    }
    shift >>= 1;
  }

  /* register the rcnumByEpnum list */
  shift = pClass->epnum;
  for(i = 0; i < USBDIF_RCNUM_TOTAL; i++) {
    if(shift & 1) {
      if(psc->rcnumByEpnum[i] != USBDIF_CLASSPOS_INVALID) goto fail;
      psc->rcnumByEpnum[i]  = num;
    }
    shift >>= 1;
  }

  result = usbdif.rcnum;
  usbdif.rcnum++;

fail:
  return result;
}


usbdifStatus_t
UsbifStart(int dev)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  result = UsbdevStart(dev);

  return result;
}


usbdifClassDef_t *
UsbifGetClassData(int drc)
{
  usbdifClassDef_t      *re = NULL;

  if(drc < 0 || drc > usbdif.rcnum) goto fail;

  re = &usbdif.rc[drc];

fail:
  return &usbdif.rc[drc];
}


void
UsbifShowSetup(usbifSetup_t *s)
{
  printf("bmReq:%02x,bReq:%02x,wVal:%04x,wIdx:%04x,wLen:%04x\r\n",
         s->bmRequest, s->bRequest,
         s->wValue, s->wIndex, s->wLength);
  if(!(s->bmRequest & 0x80) && s->wLength) {
    printf("  data: ");
    for(int i = 0; i < s->wLength; i++) {
      printf(" %02x", s->buf[i]);
    }
    printf("\r\n");
  }
  return;
}

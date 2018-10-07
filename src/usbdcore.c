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

#define _USBDCORE_C_

#include        <stdio.h>

#include        "config.h"
#include        "system.h"

#include        "usb_def.h"
#include        "usbdif.h"
#include        "usbdcore.h"


extern struct _stUsbdif         usbdif;
static uint8_t                  usbdcoreString[0x100];





#if 0
/**
  * @brief  start usb module (for STM32)
  * @param  dev USB PCD module number
  * @param  pUsbInit initialize parameters (usbdstmInitParam_t *)
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevInit(int dev, usbdifInitParam_t *pUsbInit)
{
  return 0;
}


/**
  * @brief  start usb module (for STM32)
  * @param  dev USB PCD module number
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevStart(int dev)
{
  return 0;
}
#endif




/**
  * @brief  setup event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the SETUP packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  epnum endpoint number
  * @retval result
  */
usbdifStatus_t
UsbdcoreCbBusState(int dev, usbdifBusState_t state)
{
  usbdifStatus_t        re = USBDIF_STATUS_UNKNOWN;

  struct _stUsbdifDev   *psc;
  psc = &usbdif.sc[dev];

  printf("# usbdcore.c bus state: ");

  switch(state) {
  case  USBDIF_BUSSTATE_RESET:
    printf("reset\r\n");
    UsbifCbDeInit(psc->unit, 0);
    psc->deviceState = USBDIF_DEVICESTATE_DEFAULT;
    break;
  case USBDIF_BUSSTATE_ENUMULATED_FULL:
  case USBDIF_BUSSTATE_ENUMULATED_LOW:
  case USBDIF_BUSSTATE_ENUMULATED_HIGH:
  case USBDIF_BUSSTATE_ENUMULATED_SUPER:
    printf("enumlated %x\r\n", state);
    UsbifCbInit(psc->unit, state & USBDIF_BUSSTATE_ENUMULATED_SPEED_MASK);
    break;
  case  USBDIF_BUSSTATE_RESUME:
    printf("resume\r\n");
    break;
  case  USBDIF_BUSSTATE_SUSPEND:
    printf("suspend\r\n");
    break;
  case  USBDIF_BUSSTATE_CONNECT:
    printf("connect\r\n");
    break;
  case  USBDIF_BUSSTATE_DISCONNECT:
    printf("disconnect\r\n");
    break;
  }

fail:
  return re;
}


/**
  * @brief  setup event entry from the device driver
  * @details this funcion is called from the module device driver
             if the SETUP packet is coming
  * @param  s  pointre of setup data structre
  * @retval result
  */
usbdifStatus_t
UsbdcoreCbSetup(int unit, usbifSetup_t *s)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  uint8_t               recipient;

  struct _stUsbdifDev   *psc;

  psc = &usbdif.sc[unit];

  recipient = s->bmRequest & USB_BMREQ_RECIPIENT_MASK;
  s->ptr = usbdcoreString;              /* adhoc   local buffer */
  s->len = 0;
  if(recipient == USB_BMREQ_RECIPIENT_DEVICE) {
    result = UsbdcoreSetupStandardDeviceRequest(psc, s);
  } else {
    if(psc->deviceState == USBDIF_DEVICESTATE_CONFIGURED) {
      result = UsbifCbSetup(unit, s);
    }
  }

  /* send the response packet */
  switch(result) {
  case USBDIF_STATUS_SUCCESS:
    UsbdevCtrlSendData(unit, s->ptr, s->len);
    break;
  case USBDIF_STATUS_STALL:
    UsbdevCtrlStall(unit);
    break;
  }

  return result;
}


/**
  * @brief   data received callback
  * @details this funcion is called from the module device driver
             if the data is coming
  * @param  unit  module unit number
  * @param  epnum endpoint number
  * @param  size  data size
  * @retval result
  */
usbdifStatus_t
UsbdcoreCbDataOut(int unit, uint8_t epnum, int size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  struct _stUsbdifDev   *psc;

  psc = &usbdif.sc[unit];

  if(psc->deviceState == USBDIF_DEVICESTATE_CONFIGURED) {
    result = UsbifCbDataOut(unit, epnum, size);
  }

  return result;
}
/**
  * @brief   data transmitted callback
  * @details this funcion is called from the module device driver
             if the data is finished to transmit
  * @param  unit  module unit number
  * @param  epnum endpoint number
  * @retval result
  */
usbdifStatus_t
UsbdcoreCbDataInDone(int unit, uint8_t epnum)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  struct _stUsbdifDev   *psc;

  psc = &usbdif.sc[unit];

  if(psc->deviceState == USBDIF_DEVICESTATE_CONFIGURED) {
    result = UsbifCbDataInDone(unit, epnum);
  }

  return result;
}






/**
  * @brief  setup event entry in USBDIF
  * @details this funcion is called from the module device driver
             if the SETUP packet is coming
  * @param  dev the descriptor number of the modeule device
  * @param  epnum endpoint number
  * @retval result
  */
static uint8_t
UsbdcoreSetupStandardDeviceRequest(struct _stUsbdifDev *psc, usbifSetup_t *s)
{
  usbdifStatus_t        re = USBDIF_STATUS_UNKNOWN;
  int                   num, index, classtype;
  int                   i;
  usbifClassCb_t        *cb;
  //  uint16_t              len;
  //uint8_t               *pbuf;
  usbdifClassDef_t      *prc;

  switch(s->bRequest) {
  case        USB_BREQ_GET_INTERFACE:
    re = UsbifCbSetup(psc->unit, s);

    break;
  case        USB_BREQ_SET_INTERFACE:
    printf("XXXX usbcore set 1 interface %x\r\n", s->wIndex);
    re = UsbifCbSetup(psc->unit, s);
    break;

  case        USB_BREQ_CLEAR_FEATURE:
    re = UsbifCbSetup(psc->unit, s);

    break;

  case USB_BREQ_GET_DESCRIPTOR:
    switch(s->wValue >> 8) {

    case    USB_DESC_TYPE_DEVICE:               /* Std.GetDesc.Device */
      printf("# usbdcore  device desc(busy wait)\r\n");
      s->ptr = psc->pDescTbl->device.ptr;
      s->len = MIN(psc->pDescTbl->device.len, s->wLength);
      re = USBDIF_STATUS_SUCCESS;
      break;

    case    USB_DESC_TYPE_BOS:                  /* Std.GetDesc.Bos */
      s->ptr = psc->pDescTbl->bos.ptr;
      s->len = MIN(psc->pDescTbl->bos.len, s->wLength);
      re = USBDIF_STATUS_SUCCESS;
      break;

    case    USB_DESC_TYPE_CONFIGURATION:        /* Std.GetDesc.Configuration */
      s->ptr = psc->pDescTbl->config[1][0].ptr;
      s->len = MIN(psc->pDescTbl->config[1][0].len, s->wLength);  /* adhoc  get device speed */
      re = USBDIF_STATUS_SUCCESS;
      break;

    case      USB_DESC_TYPE_DEVICE_QUALIFIER:   /* Std.GetDesc.Qualifier */
      s->ptr = psc->pDescTbl->qualifier.ptr;
      s->len = MIN(psc->pDescTbl->qualifier.len, s->wLength);
      re = USBDIF_STATUS_SUCCESS;
      break;

    case    USB_DESC_TYPE_STRING:               /* Std.GetDesc.String */
      index = s->wValue & 0xff;
      if(index == 0) {                          /* lang id */
        s->ptr = psc->pDescTbl->langId.ptr;
        s->len  = MIN(psc->pDescTbl->langId.len, s->wLength);
      } else {
        s->ptr = usbdcoreString;
        s->len = MIN(UsbdcoreBuildStringDesc(psc->pDescTbl->pString[index]), s->wLength);
      }
      re = USBDIF_STATUS_SUCCESS;
      break;

    default:
      re = USBDIF_STATUS_STALL;
    }
    break;

  case  USB_BREQ_SET_CONFIGURATION:
    if(psc->deviceState == USBDIF_DEVICESTATE_ADDRESSED) {
      psc->deviceState = USBDIF_DEVICESTATE_CONFIGURED;
    } else if(psc->deviceState == USBDIF_DEVICESTATE_CONFIGURED) {
      psc->deviceState = USBDIF_DEVICESTATE_ADDRESSED;
    }
    re = USBDIF_STATUS_SUCCESS;
    break;

  case  USB_BREQ_SET_ADDRESS:
    if(psc->deviceState == USBDIF_DEVICESTATE_DEFAULT) {
      DevUsbSetAddress(psc->unit, s->wValue);   /* only set, no response to HOST */
      psc->deviceState = USBDIF_DEVICESTATE_ADDRESSED;
      re = USBDIF_STATUS_SUCCESS;
    } else {
      re = USBDIF_STATUS_STALL;
    }
  break;

  case        USB_BREQ_GET_STATUS:
    {
      static uint8_t            buf[2] = {0}; /* adhoc    wake, power status */
      /*UsbdevCtrlSendData(psc->unit, buf, 2);*/
      s->ptr = buf;
      s->len = 2;
      re = USBDIF_STATUS_SUCCESS;
    }
    break;

  default:
    re = USBDIF_STATUS_STALL;
  }

  return re;
}


static int
UsbdcoreBuildStringDesc(uint8_t *str)
{
  int           len;
  uint8_t       *ptr, c;

  len = 0;

  ptr = usbdcoreString;
  ptr++;
  *ptr++ = USB_DESC_TYPE_STRING;
  len += 2;
  for(int i = 0; i < len; i++) {
    c = *str++;
    if(!c) break;
    *ptr++ = c;
    *ptr++ = '\0';
    len += 2;
  }
  usbdcoreString[0] = len;

  return len;
}



/**
  * @brief  send the ctrl packet
  * @param  dev: USB PCD module number
  * @param  buf: the pointer of the send data
  * @param  buf: the length of the send data
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevCtrlSendData(int unit, uint8_t *buf, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  DevUsbTransmit(unit, 0, buf, len);
  DevUsbPrepareReceive(unit, 0, NULL, 0);

  result = USBDIF_STATUS_SUCCESS;       /* adhoc */

  return result;
}

/**
  * @brief  send the ctrl packet
  * @param  dev: USB PCD module number
  * @param  buf: the pointer of the send data
  * @param  buf: the length of the send data
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevCtrlStatus(int unit)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  DevUsbTransmit(unit, 0, NULL, 0);

  result = USBDIF_STATUS_SUCCESS;       /* adhoc */

  return result;
}

/**
  * @brief  call at the control error packet will be sent
  * @param  dev  USB PCD module number
  * @param  s    the pointer of the sending setup packet
  * @retval void
  */
#if 0
void
UsbdevCtrlError(int unit, usbifSetup_t *s)
{
  DevUsbTransmit(unit, 0, s, 8);

  return;
}
#endif
static void
UsbdevCtrlStall(int unit)
{
  DevUsbTransmit(unit, 0, NULL, -1);

  return;
}

/**
  * @brief  call at sof is coming
  * @param  unit: USB PCD module number
  * @retval void
  */
void
UsbdevSofEntry(int unit)
{
  uint8_t               result = -1;
  usbdifStatus_t        re;

  int                   i;
  /*USBD_ClassTypeDef     *cb;*/
  usbifSof_t            sof;

  static uint64_t       totalCount = 0;
  static uint32_t       cntSofPrev;
  uint32_t              val;
  int                   diff, diffSof = 0;

  val = TIM2_PTR->CCR4;
  diff = cntSofPrev - val;
  if(diff > 100000) {

    diff -= 196608;
    if(abs(diff) < 20) {
      diffSof = diff;
    }
  }
  cntSofPrev = val;

  sof.masterClk = 196608000;
  sof.diff = diffSof;
  totalCount++;
  sof.totalCount = totalCount;

  re = UsbifCbSof(unit, &sof);
  if(re == USBDIF_STATUS_UNKNOWN) result = -1;

  return re;

}



/*********************************************************
 *
 */
/**
  * @brief  close the specified endpoint
  * @param  dev USB PCD module number
  * @param  epnum endpoint address (include in/out bit)
  * @param  epType endpoint type(BULK, INT, ISO)
  * @param  epMps
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevOpenEp(int unit, uint8_t epnum, uint8_t epType, uint16_t epMps)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  result = DevUsbOpenEp(unit, epnum, epType, epMps);

  return result;
}
/**
  * @brief  close the specified endpoint
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevCloseEp(int unit, uint8_t epnum)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  result = DevUsbCloseEp(unit, epnum);

  return result;
}
/**
  * @brief  prepare the recevicing buffer
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @param  pbuf: data buffer (must 32bit align)
  * @param  size: data size
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevPrepareRecv(int unit, uint8_t epnum, uint8_t *pbuf, uint16_t size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  result = DevUsbPrepareReceive(unit, epnum, pbuf, size);

  return result;
}
/**
  * @brief  send data to the specified endpoint
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @retval the number of the received data
  */
int
UsbdevGetRxDataSize(int unit, uint8_t epnum)
{
  int                   re;

  /*re = USBD_LL_GetRxDataSize(&pdev[dev], epnum);*/

  re = 0;

  return re;
}
/**
  * @brief  send data to the specified endpoint
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @param  pbuf: data buffer (must 32bit align)
  * @param  size: data size
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevTransmit(int unit, uint8_t epnum, const uint8_t *pbuf, uint16_t size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;

  result = DevUsbTransmit(unit, epnum, (uint8_t *)pbuf, size);

  return result;
}
/**
  * @brief  the speicfied endpoint fifo is flushed
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevFlush(int dev, uint8_t epnum)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
#if 0
  USBD_StatusTypeDef    re;

  re = USBD_LL_FlushEP(&pdev[dev], epnum);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;
#endif

  return result;
}

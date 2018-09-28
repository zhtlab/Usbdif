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

#define _USBDSTM_C_

#include        <usbd_ctlreq.h>
#include        <usbd_ioreq.h>


#include        "usb_def.h"
#include        "usbdif.h"
#include        "system.h"
#include        "rtos.h"

#include        "usbdstm.h"

extern USBD_ClassTypeDef        usbdAudioClassCb;


static rtosTaskId               idTask;
static rtosQueueId              idQueue;
struct _stUsbdstmBusState {
  uint8_t       state;
  uint8_t       reserved;
  uint16_t      val;
};
/* this number for (struct _stUsbdstmBusState).state */
typedef enum {
  BUSSTATE_IDLE = 0,
  BUSSTATE_RESET,
  BUSSTATE_CONNECT,
  BUSSTATE_DISCONNECT,
} usbdstmChangeBusState_t;

extern struct _stUsbdif         usbdif;


USBD_ClassTypeDef  usbdstmCompositeCbTbl = {
  UsbdstmCompositeInit,                  /* .Init */
  UsbdstmCompositeDeInit,                /* .DeInit */
  UsbdstmCompositeSetup,                 /* .Setup */
  UsbdstmCompositeEp0TxReady,            /* .EP0_TxSent */
  UsbdstmCompositeEp0RxReady,            /* .EP0_RxReady */
  UsbdstmCompositeDataInDone,            /* .DataInDone */
  UsbdstmCompositeDataOut,               /* .DataOut */
  UsbdstmCompositeSof,                   /* .SOF */
  UsbdstmCompositeIsoInIncomplete,       /* .IsoINIncomplete */
  UsbdstmCompositeIsoOutIncomplete,      /* .IsoOUTIncomplete */
  UsbdstmCompositeGetCfgDescHS,          /* .GetHSConfigDescriptor */
  UsbdstmCompositeGetCfgDescFS,          /* .GetFSConfigDescriptor */
  NULL,                                  /* .GetOtherSpeedConfigDescriptor */
  UsbdstmCompositeGetDeviceQualifierDesc,/* .GetDeviceQualifierDescriptor */
  UsbdstmCompositeGetUserStringDesc,     /* .GetUsrStrDescriptor */
};


static usbdifStringDesc_t      *pUsbdstmStringDesc;




/**
  * @brief  initialize usb upper layer
  * @details this funcion is called from the STM32 USB driver,
             if initialize
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  cfgidx index number of the configuration descriptors
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;
  struct _stUsbdstmBusState     bus;

  bus.state = BUSSTATE_CONNECT;
  bus.val = cfgidx;
  RtosQueueSendIsr(idQueue, &bus, 0);

#if 0
  re = UsbifCbInit(pdev->id, cfgidx);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;
#endif

  result = USBD_OK;

  return re;
}
/**
  * @brief  de-initialize usb upper layer
  * @details this funcion is called from the STM32 USB driver,
             if de-initialize
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  cfgidx index number of the configuration descriptors
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeDeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;
  struct _stUsbdstmBusState     bus;

#if 0
  re = UsbifCbDeInit(pdev->id, cfgidx);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;
#endif
#if 1
  bus.state = BUSSTATE_DISCONNECT;
  bus.val = cfgidx;
  RtosQueueSendIsr(idQueue, &bus, 0);
#endif

  result = USBD_OK;

  return re;
}
/**
  * @brief  setup packet is coming
  * @details this funcion is called from the STM32 USB driver,
             if the SETUP packet is coming
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  pSetup setup data
  * @retval result [USBD_OK, FAIL]
  */
static usbifSetup_t     setup[USBDIF_MAX_DEVICE];
static uint8_t
UsbdstmCompositeSetup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *pSetup)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;
  usbifSetup_t          *s;

  s = &setup[pdev->id];
  s->bmRequest = pSetup->bmRequest;
  s->bRequest  = pSetup->bRequest;
  s->wIndex    = pSetup->wIndex;
  s->wValue    = pSetup->wValue;
  s->wLength   = pSetup->wLength;

  /* the setup data is received, if the OUT and the length is greater then 0 */
  if(!(pSetup->bmRequest & 0x80) && pSetup->wLength) {
    re = USBD_CtlPrepareRx(pdev, s->buf, pSetup->wLength);
    if(re == USBD_OK) result = USBD_OK;
  } else {
    /* CTRL In or CTRL OUT without any data */
    re = UsbifCbSetup(pdev->id, s);
    if(re == USBD_OK) result = USBD_OK;
  }

  return result;
}


/**
  * @brief  EP0 rx ready event is occured
  * @details this funcion is called from the STM32 USB driver,
            if the EP0 rx ready event is occured
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeEp0RxReady(USBD_HandleTypeDef *pdev)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;

  /* call setup function, if the setup date is received */
  re = UsbifCbSetup(pdev->id, &setup[pdev->id]);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;

  return result;
}
/**
  * @brief  EP0 tx ready event is occured
  * @details this funcion is called from the STM32 USB driver,
            if the EP0 tx ready event is occured
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeEp0TxReady(USBD_HandleTypeDef *pdev)
{
  uint8_t               result = USBD_FAIL;

  result = USBD_OK;
  return result;
}


/**
  * @brief  data in event is occured
  * @details this funcion is called from the STM32 USB driver,
             if the DATA IN packet is coming
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  epnum endpoint number (include in/out bit)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeDataInDone(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;

  re = UsbifCbDataInDone(pdev->id, epnum);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;

  return re;
}
/**
  * @brief  data out event is occured
  * @details this funcion is called from the STM32 USB driver,
             if the received data is coming
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  epnum endpoint number (include in/out bit)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeDataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;
  int                   size;

  size = USBD_LL_GetRxDataSize(pdev, epnum);
  re = UsbifCbDataOut(pdev->id, epnum, size);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;

  return re;
}
/**
  * @brief  sof patcket is received
  * @details this funcion is called from the STM32 USB driver,
             if the SOF packet is received.
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeSof(USBD_HandleTypeDef *pdev)
{
  uint8_t               result = USBD_FAIL;
  usbdifStatus_t        re;

  int                   i;
  USBD_ClassTypeDef     *cb;
  usbifSof_t            sof;

  static uint64_t       totalCount = 0;
  static uint32_t       cntSofPrev;
  uint32_t              val;
  int                   diff, diffSof = 0;

  val = TIM2->CCR4;
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

  re = UsbifCbSof(pdev->id, &sof);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;

  return re;
}
/**
  * @brief  called, when the event of ISO IN incomplete
  * @details this funcion is called from the STM32 USB driver,
             if the send data is not exist when the iso in packet is received
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  epnum endpoint number (include in/out bit)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeIsoInIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t               result = USBD_FAIL;

#if 0
  usbdifStatus_t        re;
  int                   num;
  struct _stUsbdifDev   *psc;
  USBD_ClassTypeDef     *cb;

  /* jump to corresponding class which has "epnum" EP */
  psc = &usbdif.sc[pdev->id];
  num = psc->classTypeByEpnum[epnum] + USBDIF_RCNUM_OFFSET_IN; /* upper 16bit is for EP IN bits */
  cb = usbdstmCompositeClassCbTbl[num];
  if(cb) re = cb->IsoINIncomplete(pdev, epnum);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;
#endif

  result = USBD_OK;
  return result;
}
/**
  * @brief  called, when the event of ISO OUT incomplete
  * @details this funcion is called from the STM32 USB driver,
             if the receive fifo is not empty when the iso out packet is received
  * @param  pdev the pointer of STM32 USB driver (USBD_HandleTypeDef *)
  * @param  epnum endpoint number (include in/out bit)
  * @retval result [USBD_OK, FAIL]
  */
static uint8_t
UsbdstmCompositeIsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  uint8_t               result = USBD_FAIL;

#if 0
  usbdifStatus_t        re;
  int                   num;
  struct _stUsbdifDev   *psc;
  USBD_ClassTypeDef     *cb;

  /* jump to corresponding class which has "epnum" EP */
  psc = &usbdif.sc[pdev->id];
  num = psc->classTypeByEpnum[epnum];   /* lower 16bit is for EP OUT bits */
  cb = usbdstmCompositeClassCbTbl[num];
  if(cb) re = cb->IsoOUTIncomplete(pdev, epnum);
  if(re == USBDIF_STATUS_UNKNOWN) result = USBD_OK;
#endif

  result = USBD_OK;
  return result;
}
/**
  * @brief  get the configuration descriptor for SS
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor
  */
static uint8_t
*UsbdstmCompositeGetCfgDescHS(uint16_t *length)
{
  uint8_t               *p = USBDIF_NULL;

  if(UsbifCbGetConfigDesc(0, &p, length, USBD_SPEED_HIGH) != USBDIF_STATUS_SUCCESS) {
    *length = 0;
    p = USBDIF_NULL;
  }

  return p;
}
/**
  * @brief  get the configuration descriptor for FS
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor
  */
static uint8_t
*UsbdstmCompositeGetCfgDescFS(uint16_t *length)
{
  uint8_t               *p = USBDIF_NULL;

  if(UsbifCbGetConfigDesc(0, &p, length, USBD_SPEED_FULL) != USBDIF_STATUS_SUCCESS) {
    *length = 0;
    p = USBDIF_NULL;
  }

  return p;
}
/**
  * @brief  get the qualifier descriptor
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor
  */
static uint8_t *
UsbdstmCompositeGetDeviceQualifierDesc(uint16_t *length)
{
  uint8_t               *p = USBDIF_NULL;

  if(UsbifCbGetQualifierDesc(0, &p, length, USBD_SPEED_FULL) != USBDIF_STATUS_SUCCESS) {
    *length = 0;
    p = USBDIF_NULL;
  }

  return p;
}

__ALIGN_BEGIN uint8_t USBD_StrDesc[USB_STR_DESC_MAXSIZE] __ALIGN_END;

/**
  * @brief  get the user string descriptor
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor
  */
static uint8_t *
UsbdstmCompositeGetUserStringDesc(USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length)
{
  uint8_t               *re = USBDIF_NULL;
  int                   unit;
  struct _stUsbdifDev   *psc;

  /* adhoc   call the upper application */
#define USB_STRING_INDEX_MACADDRESS                     6       /* adhoc */
  switch(index) {
  case        USB_STRING_INDEX_MACADDRESS:
    /*USBD_GetString(usbDescStringMacAddress, USBD_StrDesc, length);*/
    USBD_GetString("aa1122334455", USBD_StrDesc, length);

    printf("XXX %x\r\n", index);

    break;
  default:
    *length = 0;
    USBD_StrDesc[0] = 0;
  }

fail:
  return USBD_StrDesc;
}


/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the device descriptor
  */
static uint8_t *
UsbdstmDeviceDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  int                   unit = 0;
  uint8_t               *p = USBDIF_NULL;

  struct _stUsbdifDev   *psc;

  *length = 0;
  if(unit < 0 || unit >= USBDIF_MAX_DEVICE) goto fail;

  psc = &usbdif.sc[unit];

  *length = psc->initParam.lenDeviceDesc;
  p       = psc->initParam.pDeviceDesc;

fail:
  return p;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for language id
  */
static uint8_t *
UsbdstmLangIDStrDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->langId;

  if(pUsbdstmStringDesc && p) {
    *length =  *p;      /* the first of buffer is set the descriptor size */
  } else {
    *length = 0;
    p = USBDIF_NULL;
  }

  return p;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for product
  */
static uint8_t *
UsbdstmProductStrDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->product;

  if(pUsbdstmStringDesc && p) {
    USBD_GetString(p, USBD_StrDesc, length);
  } else {
    *length = 0;
    USBD_StrDesc[0] = '\0';
  }

  return USBD_StrDesc;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for manufacturer
  */
static uint8_t *
UsbdstmManufacturerStrDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->manufacturer;

  if(pUsbdstmStringDesc && p) {
    USBD_GetString(p, USBD_StrDesc, length);
  } else {
    *length = 0;
    USBD_StrDesc[0] = '\0';
  }

  return USBD_StrDesc;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for the serial number
  */
static uint8_t *
UsbdstmSerialStrDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->serialnumber;

  if(pUsbdstmStringDesc && p) {
    USBD_GetString(p, USBD_StrDesc, length);
  } else {
    *length = 0;
    USBD_StrDesc[0] = '\0';
  }

  return USBD_StrDesc;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for configuration
  */
static uint8_t *
UsbdstmConfigStrDescriptor(USBD_SpeedTypeDef speed , uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->configuration;

  if(pUsbdstmStringDesc && p) {
    USBD_GetString(p, USBD_StrDesc, length);
  } else {
    *length = 0;
    USBD_StrDesc[0] = '\0';
  }

  return USBD_StrDesc;
}
/**
  * @brief  start usb module (for STM32)
  * @param  speed interface speed setting (LS, FS, HS, SS)
  * @param  length the pointer of the length
  * @retval the pointer of the string descriptor for interface
  */
static uint8_t *
UsbdstmInterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length)
{
  uint8_t               *p;

  p = pUsbdstmStringDesc->interface;

  if(pUsbdstmStringDesc && p) {
    USBD_GetString(p, USBD_StrDesc, length);
  } else {
    *length = 0;
    USBD_StrDesc[0] = '\0';
  }

  return USBD_StrDesc;
}




USBD_DescriptorsTypeDef         usbDescFuncTbl =
{
  UsbdstmDeviceDescriptor,
  UsbdstmLangIDStrDescriptor,
  UsbdstmManufacturerStrDescriptor,
  UsbdstmProductStrDescriptor,
  UsbdstmSerialStrDescriptor,
  UsbdstmConfigStrDescriptor,
  UsbdstmInterfaceStrDescriptor,
};

USBD_HandleTypeDef      pdev[USBDIF_MAX_DEVICE];

/**
  * @brief  start usb module (for STM32)
  * @param  dev USB PCD module number
  * @param  pUsbInit initialize parameters (usbdstmInitParam_t *)
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevInit(int dev, usbdifInitParam_t *pUsbInit)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  struct _stUsbdifDev   *psc;

  if(dev < 0 || dev >= USBDIF_MAX_DEVICE) goto fail;

  psc = &usbdif.sc[dev];
  /* copy the string descritpors */
  pUsbdstmStringDesc = &pUsbInit->pStringDescTbl[0];

  /* create task */
  rtosTaskInfo_t  taskInfo = {
    .pFunc = (rtosTaskFunc)UsbdevTask,
    .pName = NULL,
    .priority = RTOS_PRI_NORMAL,
    .szStack = 0x800,
  };
  idTask = RtosTaskCreate(&taskInfo, NULL);

  USBD_Init(&pdev[dev], &usbDescFuncTbl, dev);

  psc->speed = USBIF_SPEED_FULL;

fail:
  return result;
}


/**
  * @brief  start usb module (for STM32)
  * @param  dev USB PCD module number
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevStart(int dev)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  struct _stUsbdifDev   *psc;

  if(dev < 0 || dev >= USBDIF_MAX_DEVICE) goto fail;

  psc = &usbdif.sc[dev];
  if(psc->classnum == 0) goto fail;

  /* register the composite device to the usb core */
  USBD_RegisterClass(&pdev[dev], (USBD_ClassTypeDef *)&usbdstmCompositeCbTbl);
  USBD_Start(&pdev[dev]);

  /* power control usb phy 3.3V power enable
   * see document section 6 PWR, PWR_CR3 */
  PWR->CR3 |= PWR_CR3_USB33DEN;

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}


static void
UsbdevTask(const void *arg)
{
  rtosStatus_t                  re;
  struct _stUsbdstmBusState     bus;

  idQueue = RtosQueueCreate(4, sizeof(struct _stUsbdstmBusState));

  while(1) {
    re = RtosQueueRecv(idQueue, &bus, 100);
    if(re == RTOS_TIMEOUT) {
      /*printf("usbdevTask\r\n");*/
    } else {
      printf("usbdevTask state changed %x\r\n", bus.state);
      switch(bus.state) {
      case    BUSSTATE_RESET:
        break;
      case    BUSSTATE_CONNECT:
        UsbifCbInit(pdev->id, bus.val);
        break;
      case    BUSSTATE_DISCONNECT:
        UsbifCbDeInit(pdev->id, bus.val);
        break;
      }
    }
  }
  return;
}





/**
  * @brief  send the ctrl packet
  * @param  dev: USB PCD module number
  * @param  buf: the pointer of the send data
  * @param  buf: the length of the send data
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevCtrlSendData(int dev, uint8_t *buf, uint16_t len)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  USBD_StatusTypeDef    re;

  re = USBD_CtlSendData(&pdev[dev], buf, len);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

  return result;
}

/**
  * @brief  call at the control error packet will be sent
  * @param  dev  USB PCD module number
  * @param  s    the pointer of the sending setup packet
  * @retval void
  */
void
UsbdevCtrlError(int dev, usbifSetup_t *s)
{
  USBD_SetupReqTypedef          setup;
  setup.bmRequest = s->bmRequest;
  setup.bRequest = s->bRequest;
  setup.wValue = s->wValue;
  setup.wIndex = s->wIndex;
  setup.wLength = s->wLength;

  USBD_CtlError(&pdev[dev], &setup);

  return;
}
/**
  * @brief  call at sof is coming
  * @param  dev: USB PCD module number
  * @retval void
  */
void
UsbdevSofEntry(int dev)
{
  UsbdstmCompositeSof(&pdev[dev]);

  return;
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
UsbdevOpenEp(int dev, uint8_t epnum, uint8_t epType, uint16_t epMps)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  USBD_StatusTypeDef    re;

  re = USBD_LL_OpenEP(&pdev[dev], epnum, epType, epMps);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

  return result;
}
/**
  * @brief  close the specified endpoint
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @retval usbdifStatus_t
  */
usbdifStatus_t
UsbdevCloseEp(int dev, uint8_t epnum)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  USBD_StatusTypeDef    re;

  re = USBD_LL_CloseEP(&pdev[dev], epnum);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

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
UsbdevPrepareRecv(int dev, uint8_t epnum, uint8_t *pbuf, uint16_t size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  USBD_StatusTypeDef    re;

  re = USBD_LL_PrepareReceive(&pdev[dev], epnum, pbuf, size);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

  return result;
}
/**
  * @brief  send data to the specified endpoint
  * @param  dev: USB PCD module number
  * @param  epnum: endpoint address (include in/out bit)
  * @retval the number of the received data
  */
int
UsbdevGetRxDataSize(int dev, uint8_t epnum)
{
  int                   re;

  re = USBD_LL_GetRxDataSize(&pdev[dev], epnum);

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
UsbdevTransmit(int dev, uint8_t epnum, const uint8_t *pbuf, uint16_t size)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  USBD_StatusTypeDef    re;

  re = USBD_LL_Transmit(&pdev[dev], epnum, (uint8_t *)pbuf, size);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

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
  USBD_StatusTypeDef    re;

  re = USBD_LL_FlushEP(&pdev[dev], epnum);
  if(re == USBD_OK) result = USBDIF_STATUS_SUCCESS;

  return result;
}

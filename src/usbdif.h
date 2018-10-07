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

#ifndef _USBDIF_H_
#define _USBDIF_H_

#include        <stdint.h>
#include        <stdlib.h>
#include        <string.h>


#define USBDIF_MAX_DEVICE       3       /* FS module, HS module */
#define USBDIF_MAX_REGCLASS     5
#define USBDIF_MAX_EPNUM_BIT    4
#define USBDIF_MAX_EPNUM_BITMASK        ((1<<(USBDIF_MAX_EPNUM_BIT))-1)
#define USBDIF_MAX_EPNUM        (1<<(USBDIF_MAX_EPNUM_BIT))


typedef enum {
  USBDIF_STATUS_SUCCESS         = 0,    /* zero or positive number is success */
  USBDIF_STATUS_UNKNOWN         = (-1),
  USBDIF_STATUS_BUSY            = (-2),
  USBDIF_STATUS_MEMORY          = (-3),
  USBDIF_STATUS_INVALIDARG      = (-4),
  USBDIF_STATUS_OUTOFRANGE      = (-5),
  USBDIF_STATUS_STALL           = (-100)
} usbdifStatus_t;



#define USBDIF_EPBIT(x)                 (1 << (((x)&((USBDESC_EP_NUM_MASK)))+  (((x)&(USBDESC_EP_DIR_MASK))?16:0)))

#define USBDIF_NULL                     ((void *)0)

typedef enum {
  USBDIF_DEVICESTATE_DEFAULT = 0,
  USBDIF_DEVICESTATE_ADDRESSED,
  USBDIF_DEVICESTATE_CONFIGURED,
  USBDIF_DEVICESTATE_SUSPENDED_DEFAULT,
  USBDIF_DEVICESTATE_SUSPENDED_ADDRESSED,
  USBDIF_DEVICESTATE_SUSPENDED_CONFIGURED,
} usbdifDeviceState_t;

#if 0
enum usbdifPhySpeed {
  USBIF_SPEED_FULL        = USB_SPEED_FULL,
  USBIF_SPEED_LOW         = USB_SPEED_LOW,
  USBIF_SPEED_HIGH        = USB_SPEED_HIGH,
  USBIF_SPEED_SUPER       = USB_SPEED_SUPER
};
#endif
/* USB_SPEED_XX are defined in the device definition file */
#define USBIF_SPEED_FULL        USB_SPEED_FULL
#define USBIF_SPEED_LOW         USB_SPEED_LOW
#define USBIF_SPEED_HIGH        USB_SPEED_HIGH
#define USBIF_SPEED_SUPER       USB_SPEED_SUPER

typedef enum {
  USBDIF_BUSSTATE_INVALID = 0,
  USBDIF_BUSSTATE_RESUME,
  USBDIF_BUSSTATE_SUSPEND,
  USBDIF_BUSSTATE_CONNECT,
  USBDIF_BUSSTATE_DISCONNECT,
  USBDIF_BUSSTATE_RESET,
  USBDIF_BUSSTATE_ENUMULATED       = 0x10,
} usbdifBusState_t;
#define USBDIF_BUSSTATE_ENUMULATED_SPEED_MASK (0xf)
#define USBDIF_BUSSTATE_ENUMULATED_FULL  ((USBDIF_BUSSTATE_ENUMULATED) | (USBIF_SPEED_FULL))
#define USBDIF_BUSSTATE_ENUMULATED_LOW   ((USBDIF_BUSSTATE_ENUMULATED) | (USBIF_SPEED_LOW))
#define USBDIF_BUSSTATE_ENUMULATED_HIGH  ((USBDIF_BUSSTATE_ENUMULATED) | (USBIF_SPEED_HIGH))
#define USBDIF_BUSSTATE_ENUMULATED_SUPER ((USBDIF_BUSSTATE_ENUMULATED) | (USBIF_SPEED_SUPER))

typedef enum {
  USBDIF_CLASSTYPE_RESERVED0    = 0,
  USBDIF_CLASSTYPE_AUDIO,
  USBDIF_CLASSTYPE_CDC,
  USBDIF_CLASSTYPE_HID,
  USBDIF_CLASSTYPE_RESERVED4,
  USBDIF_CLASSTYPE_PHYSICAL,
  USBDIF_CLASSTYPE_IMAGE,
  USBDIF_CLASSTYPE_PRINTER,
  USBDIF_CLASSTYPE_MASS_STORAGE,
  USBDIF_CLASSTYPE_HUB,
  USBDIF_CLASSTYPE_CDC_DATA,
  USBDIF_CLASSTYPE_SMART_CARD,
  USBDIF_CLASSTYPE_SECURITY,
  USBDIF_CLASSTYPE_VIDEO,
  USBDIF_CLASSTYPE_END_MAJOR,
  USBDIF_CLASSTYPE_DIAG         = 0xdc,
  USBDIF_CLASSTYPE_WIRESS       = 0xe0,
  USBDIF_CLASSTYPE_APP_SPECIFIC = 0xfe,
  USBDIF_CLASSTYPE_VENDOR       = 0xff,
} usbdifClassType_t;

typedef struct {
  int           sizeRx[16];
  int           sizeTx[16];
} usbdifDevFifo_t;


typedef struct {
  uint8_t       bmRequest;
  uint8_t       bRequest;
  uint16_t      wValue;
  uint16_t      wIndex;
  uint16_t      wLength;
  uint8_t       buf[64];
  /* for response */
  uint8_t       *ptr;   /* it must store the pointer of the global array */
  uint16_t      len;
} usbifSetup_t;


typedef struct {
  uint64_t      totalCount;     /* total count */
  int           masterClk;
  int           diff;
} usbifSof_t;


typedef struct {
  /*uint8_t     *device;*/
  uint8_t       *langId;
  uint8_t       *manufacturer;
  uint8_t       *product;
  uint8_t       *serialnumber;
  uint8_t       *configuration;
  uint8_t       *interface;
} usbdifStringDesc_t;


typedef struct {
  uint8_t               *pDeviceDesc;
  int                   lenDeviceDesc;
  uint8_t               *pDeviceQualifierDesc;
  int                   lenDeviceQualifierDesc;
  uint8_t               *pConfigDesc;
  int                   lenConfigDesc;
  int                   numConfigDesc;
  usbdifStringDesc_t    pStringDescTbl[4];
} usbdifInitParam_t;

typedef struct {
  uint8_t               *ptr;
  int                   len;
} usbdifDescriptor_t;
typedef struct {
  usbdifDescriptor_t    device;
  usbdifDescriptor_t    bos;
  usbdifDescriptor_t    qualifier;
  usbdifDescriptor_t    config[4][2];   /* 1st index= 0:LS, 1:FS, 2:HS, 3:SS, 2nd=conf number */
  uint8_t               *pString[10];
  usbdifDescriptor_t    langId;
} usbdifDescritprTbl_t;




#define USBIF_EP_CTRL           0
#define USBIF_EP_ISOC           1
#define USBIF_EP_BULK           2
#define USBIF_EP_INTR           3



typedef struct _stUsbstmRegisterClass {
  /* filled by user */
  uint8_t               numClass;       /* class number */
  usbdifClassType_t     type;
  uint32_t              epnum;          /* set corresponding bit */
  uint32_t              ifnum;          /* set corresponding bit */

  /* internal use */
  uint8_t               dev;            /* device number */
  int                   (*cb)(void);

  void                  *pClassData;    /* the buffer is reserved by each class */
  void                  *pUserData;     /* this variable is pointed to external structure */
} usbdifClassDef_t;





struct _stUsbdifDev {
  uint8_t               up;
  uint8_t               unit;
  uint8_t               deviceState;
  uint8_t               speed;          /* usbdifPhySpeed_t */
#if 0
#define USBIF_SPEED_FULL        0
#define USBIF_SPEED_LOW         1
#define USBIF_SPEED_HIGH        2
#define USBIF_SPEED_SUPER       3
#endif

  usbdifInitParam_t     initParam;
  const usbdifDescritprTbl_t    *pDescTbl;

  /* classNum and classType array */
#define USBDIF_CLASSPOS_INVALID         (0xff)
  /* classnum: next unused position of rcnumByClassnum[] in each module */
  int                   classnum;
#define USBDIF_RCNUM_TOTAL              (32)
#define USBDIF_RCNUM_OFFSET_OUT         (0)
#define USBDIF_RCNUM_OFFSET_IN          (16)
  uint8_t               rcnumByClassnum[USBDIF_RCNUM_TOTAL];

  /* interface number to rcnumber converter table */
  uint8_t               rcnumByIfnum[USBDIF_RCNUM_TOTAL];
  /* end point number to rcnumber converter table  [31--16]:in, [15--0]:out */
  uint8_t               rcnumByEpnum[USBDIF_RCNUM_TOTAL];
};


struct _stUsbdif {
  /*
   * device (module) information
   *   USBDIF_MAX_DEVICE is 2, if the usb device module has 2 modules in a chip
   */
  struct _stUsbdifDev   sc[USBDIF_MAX_DEVICE];

  /*
   * registered class information
   * include all devices (modules)
   */
  usbdifClassDef_t      rc[USBDIF_MAX_REGCLASS];
  int                   rcnum;
};



typedef struct {
  usbdifStatus_t  (*init)          (usbdifClassDef_t *prc, int speed);
  usbdifStatus_t  (*deinit)        (usbdifClassDef_t *prc, int speed);
  /* Control Endpoints*/
  usbdifStatus_t  (*setup)         (usbdifClassDef_t *prc, usbifSetup_t *req);
  usbdifStatus_t  (*dataInDone)    (usbdifClassDef_t *prc, uint8_t epnum);
  usbdifStatus_t  (*dataOut)       (usbdifClassDef_t *prc, uint8_t epnum, int size);
  usbdifStatus_t  (*sof)           (usbdifClassDef_t *prc, usbifSof_t *sof);
} usbifClassCb_t;


uint8_t         UsbifCbInit(int dev, int speed);
uint8_t         UsbifCbDeInit(int dev, int speed);
uint8_t         UsbifCbBusState(int dev, usbdifBusState_t state);
uint8_t         UsbifCbSetup(int dev, usbifSetup_t *pSetup);
uint8_t         UsbifCbDataOut(int dev, uint8_t epnum, int size);
uint8_t         UsbifCbDataInDone(int dev, uint8_t epnum);
uint8_t         UsbifCbSof(int dev, usbifSof_t *sof);
uint8_t         UsbifCbGetConfigDesc(int dev, uint8_t **ppDesc, uint16_t *pLen, int speed);
uint8_t         UsbifCbGetQualifierDesc(int dev, uint8_t **ppDesc, uint16_t *pLen, int speed);


usbdifStatus_t  UsbifInit(int dev, usbdifInitParam_t *pUsbInit);
int             UsbifRegisterClass(int dev, const usbdifClassDef_t *pClass);
usbdifStatus_t  UsbifStart(int dev);
usbdifClassDef_t        *UsbifGetClassData(int drc);

void            UsbifShowSetup(usbifSetup_t *setup);


/*
 * USB driver
 */
usbdifStatus_t          UsbdevInit(int dev, usbdifInitParam_t *pUsbInit);
usbdifStatus_t          UsbdevStart(int dev);
usbdifStatus_t          UsbdevOpenEp(int dev, uint8_t  ep_addr, uint8_t  epType, uint16_t epMps);
usbdifStatus_t          UsbdevCloseEp(int dev, uint8_t ep_addr);
usbdifStatus_t          UsbdevPrepareRecv(int dev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size);
int                     UsbdevGetRxDataSize(int dev, uint8_t ep_addr);
usbdifStatus_t          UsbdevTransmit(int dev, uint8_t ep_addr, const uint8_t *pbuf, uint16_t size);
usbdifStatus_t          UsbdevFlush(int dev, uint8_t ep_addr);
usbdifStatus_t          UsbdevCtrlSendData(int dev, uint8_t *buf, uint16_t len);
void                    UsbdevCtrlError(int dev, usbifSetup_t *s);
void                    UsbdevSofEntry(int dev);



#ifdef  _USBDIF_C_
static uint8_t          UsbifSetupStandardDeviceRequest(struct _stUsbdifDev *psc, usbifSetup_t *s);

#endif

#define UsbdifMalloc(x)           malloc(x)
#define UsbdifFree(x)             free(x)

#undef  LOBYTE
#undef  HIBYTE
#define LOBYTE(x)       (((x)     ) & 0xff)
#define HIBYTE(x)       (((x) >> 8) & 0xff)
#define MIN(a, b)       (((a) < (b)) ? (a) : (b))
#define MAX(a, b)       (((a) > (b)) ? (a) : (b))



#if defined   (__GNUC__)        /* GNU */
#define __ALIGN_END             __attribute__ ((aligned (4)))
#define __ALIGN_BEGIN

#else
#define __ALIGN_END
#if defined   (__CC_ARM)        /* KEIL */
#define __ALIGN_BEGIN           __align(4)

#elif defined (__ICCARM__)      /* IAR */
#define __ALIGN_BEGIN
#endif

#endif /* __GNUC__ */




#endif

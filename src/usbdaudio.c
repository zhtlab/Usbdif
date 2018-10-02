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

#define _USBDAUDIO_C_


#include        "config.h"

#include        "usb_def.h"
#include        "usb_def_audio20.h"

#include        "usb_desc.h"
#include        "usbdif.h"
#include        "usbdaudio.h"

#include        "system.h"



extern struct _stUsbdif         usbdif;

const static uint8_t            usbdaudioZeros[0xc0] = {};
static uint8_t                  bufFb[4];


usbifClassCb_t  usbdAudioClassCb = {
  UsbdAudioCbInit,
  UsbdAudioCbDeInit,
  UsbdAudioCbSetup,
  UsbdAudioCbDataIn,
  UsbdAudioCbDataOut,
  UsbdAudioCbSof,
};


#define SOF_LIST_POW    (10)     /* this value must be >= 5 */
#define SOF_LIST_LEN    (1<<SOF_LIST_POW)
static int              accSof = 0;
static int8_t           bufSof[SOF_LIST_LEN];
static int              posSof = 0;


/**
  * @brief  callback entry of init
  * @details this function is called, if initialize
  * @param  drc  registered class number
  * @param  cfgidx ???
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbInit(usbdifClassDef_t *prc, int speed)
{
  uint8_t               result = USBDIF_STATUS_UNKNOWN;
  usbdAudioHandle_t     *pAudio;

  memset(bufSof, 0, sizeof(bufSof));

  /* Allocate Audio structure */
  if(!prc->pClassData) {
    prc->pClassData = UsbdifMalloc(sizeof(usbdAudioHandle_t));
    if(prc->pClassData == NULL) goto fail;
    memset(prc->pClassData, 0, sizeof(usbdAudioHandle_t));
  }

  pAudio = (usbdAudioHandle_t*)prc->pClassData;

  /* adhoc */
  /* this codes should be set by the upper function */
  pAudio->ifCtrl = USBDESC_IF_AUDIO_CTRL; /* adhoc */
  pAudio->ifOut  = USBDESC_IF_AUDIO_OUT; /* adhoc */
  pAudio->ifIn   = USBDESC_IF_AUDIO_IN; /* adhoc */

  pAudio->epOut = USBDAUDIO_EP_OUT; /* adhoc */
  pAudio->epFb  = USBDAUDIO_EP_FB; /* adhoc */
  pAudio->epIn  = USBDAUDIO_EP_IN; /* adhoc */

  pAudio->pktDefaultSizeOut = AUDIO_INOUT_PACKET_SIZE; /* adhoc */
  pAudio->pktDefaultSizeIn  = AUDIO_INOUT_PACKET_SIZE; /* adhoc */
  pAudio->epSizeOut = pAudio->pktDefaultSizeOut + 4;  /* adhoc  4 is size per sample */
  pAudio->epSizeIn  = pAudio->pktDefaultSizeIn + 4;  /* adhoc  4 is size per sample */

  pAudio->outBufSize = pAudio->pktDefaultSizeOut * 16;
  if(!pAudio->outBufPtr) {
    pAudio->outBufPtr = SystemMallocStreamBuf(0, pAudio->outBufSize + pAudio->epSizeOut, (void *)0);
  }
  pAudio->outBufPosWr = 0;
  pAudio->outBufPosRd = 0;

  /* Open EP OUT */
#if 0
  UsbdevOpenEp(prc->dev, pAudio->epOut, USBIF_EP_ISOC, pAudio->epSizeOut);
  UsbdevOpenEp(prc->dev, pAudio->epFb,  USBIF_EP_ISOC, 4);
  UsbdevOpenEp(prc->dev, pAudio->epIn, USBIF_EP_ISOC, pAudio->epSizeIn);
#endif

  /* Initialize the Audio output Hardware layer */
  if(((usbdAudioCb_t *)prc->pUserData)->init(CONFIG_AUDIO_FS_48K000, AUDIO_DEFAULT_VOLUME, 0) != USBDIF_STATUS_SUCCESS) goto fail;

  /* Prepare receiving 1st packet on out ep */
  UsbdevPrepareRecv(prc->dev, pAudio->epOut, pAudio->outBufPtr, pAudio->epSizeOut);

  result = USBDIF_STATUS_SUCCESS;
fail:
  return result;
}
/**
  * @brief  callback entry of de-init
  * @details this function is called, if de-initialize
  * @param  drc  registered class number
  * @param  cfgidx ???
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbDeInit(usbdifClassDef_t *prc, int speed)
{
  usbdAudioHandle_t     *pAudio;

  pAudio = (usbdAudioHandle_t *)prc->pClassData;

  /* Close EP IN */
  UsbdevCloseEp(prc->dev, pAudio->epIn);
  /* Close EP OUT */
  UsbdevCloseEp(prc->dev, pAudio->epOut);
  UsbdevCloseEp(prc->dev, pAudio->epFb);

  /* DeInit  physical Interface components */
  if(prc->pClassData != NULL) {
    ((usbdAudioCb_t *)prc->pUserData)->deinit(0);
    UsbdifFree(prc->pClassData);
    prc->pClassData = NULL;
  }

  return USBDIF_STATUS_SUCCESS;
}

usbdifStatus_t          UsbAudioSetup(int drc, usbifSetup_t *s);

uint8_t buf[64];
/**
  * @brief  callback entry of receiving the SETUP packet
  * @details this function is called, if the SETUP packet is received
  * @param  drc  registered class number
  * @param  s the structure data of setup packet
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  usbdifStatus_t        re = USBDIF_STATUS_SUCCESS;

  usbdAudioHandle_t     *pAudio;
  uint16_t              len;
  uint8_t               *pbuf, val;
  int                   dev;

  dev = prc->dev;
  pAudio = (usbdAudioHandle_t*)prc->pClassData;

  switch (s->bmRequest & USB_BMREQ_TYPE_MASK) {
  case USB_BMREQ_TYPE_CLASS:
    re = UsbAudioSetup(prc->numClass, s);
    break;

  case USB_BMREQ_TYPE_STANDARD:
    switch (s->bRequest) {
      /*** interface control */
    case USB_BREQ_GET_INTERFACE:
      re = UsbdAudioStandardGetInterface(prc, s);
      break;

    case USB_BREQ_SET_INTERFACE:
      re = UsbdAudioStandardSetInterface(prc, s);
      break;

    default:
      re = USBDIF_STATUS_STALL;
    }
    break;
  default:
    re = USBDIF_STATUS_STALL;
  }

  return re;
}


/**
  * @brief  callback entry of receiving the EP IN token packet
  * @details this function is called, if the EP IN token  packet is received
  * @param  drc  registered class number
  * @param  epnum the ep number of received
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbDataIn(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdAudioHandle_t     *pAudio;

  pAudio = (usbdAudioHandle_t *) prc->pClassData;

  if(epnum == (pAudio->epIn & 0x7f)) {
    if(pAudio && pAudio->altIn) {
      static  uint32_t bufxx[0xc0];
      memset(bufxx, 0x04, sizeof(bufxx));       /* adhoc */
      UsbdevTransmit(prc->dev, USBDESC_EP_AUDIO_IN, (uint8_t*)bufxx, AUDIO_INOUT_PACKET_SIZE);
    }
  } else if(epnum == (pAudio->epFb & 0x7f)) {
    if(pAudio && pAudio->altCtrl) {
      UsbdevTransmit(prc->dev, pAudio->epFb, bufFb, 4);
    }
  }

  return USBDIF_STATUS_SUCCESS;
}
/**
  * @brief  callback entry of receiving the EP data packet
  * @details this function is called, if the EP data packet is received
  * @param  drc  registered class number
  * @param  epnum the ep number of received
  * @param  size recevied data size
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size)
{
  usbdAudioHandle_t     *pAudio;
  int                   empty;

  pAudio = (usbdAudioHandle_t *) prc->pClassData;

  if(epnum == pAudio->epOut) {
    pAudio->outBufPosWr += size;
    if(pAudio->outBufPosWr >= pAudio->outBufSize) {
      uint8_t   *d, *s;
      int       sz;
      d = &pAudio->outBufPtr[0];
      s = &pAudio->outBufPtr[pAudio->outBufSize];
      sz = pAudio->outBufPosWr - pAudio->outBufSize;
      memcpy(d, s, sz);
      pAudio->outBufPosWr = sz;
    }
    if(pAudio->outBufPosWr > pAudio->outBufPosRd) {
      empty = pAudio->outBufSize + pAudio->outBufPosWr - pAudio->outBufPosRd;
    } else {
      empty = pAudio->outBufPosRd - pAudio->outBufPosWr;
    }

    /* flush the oldest data, if overrun */
    if(empty < pAudio->epSizeOut) {
      pAudio->outBufPosRd += pAudio->epSizeOut;
      if(pAudio->outBufPosRd >= pAudio->outBufSize) {
        pAudio->outBufPosRd = pAudio->outBufPosRd - pAudio->outBufSize;
      }
      /* call back       adhoc*/
      /*printf("audio overrun %x\r\n", size);*/
    }

    /* Prepare Out endpoint to receive next audio packet */
    UsbdevPrepareRecv(prc->dev, pAudio->epOut,
                    &pAudio->outBufPtr[pAudio->outBufPosWr], pAudio->epSizeOut);
  }

  return USBDIF_STATUS_SUCCESS;
}


/**
  * @brief  callback entry of receiving the SOF packet
  * @details this function is called, if the SETUP packet is received
  * @param  drc  registered class number
  * @param  sof the structure data of the sof information
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioCbSof(usbdifClassDef_t *prc, usbifSof_t *sof)
{
  static int            i = 0;
  static int            cntFb = 0;
  int                   diff;
  uint32_t              valFb;

  usbdAudioHandle_t     *pAudio;

  pAudio = (usbdAudioHandle_t *) prc->pClassData;

  diff = sof->diff;

  accSof += diff;
  accSof -= bufSof[posSof];
  bufSof[posSof] = diff;
  posSof++;
  posSof &= (SOF_LIST_LEN-1);

  valFb   = accSof + 48*1024*SOF_LIST_LEN;      /* 196608 = 48*4096 */

#define SHIFT   (12+SOF_LIST_POW-16)
#if     (SHIFT > 0)
  valFb  += 1<<(SHIFT-1);
  valFb >>= SHIFT;
#elif   (SHIFT < 0)
  valFb <<= -SHIFT;
#endif

  /* shift more 2 bit, if the usb speed is FS
   *   HS: 16.16 format
   *   FS: 10.14 format
   */
  if(1) {
    valFb  += 2;
    valFb >>= 2;
  }

  if(pAudio->altOut) {
    if(i > 250) {
      bufFb[0] =  valFb        & 0xff;
      bufFb[1] = (valFb >>  8) & 0xff;
      bufFb[2] = (valFb >> 16) & 0xff;
      bufFb[3] = (valFb >> 24) & 0xff;
      UsbdevTransmit(prc->dev, pAudio->epFb, bufFb, 4);
      i = 0;
    }
  }
  i++;


#if 0
  {
    static int i = 0;
    if(i > 256) {
      printf("xx %d %x %x %x %d\r\n", cntFb, sof->masterClk, accSof, valFb, valFb);
      i = 0;
      cntFb++;
    }
    i++;
  }
#endif

  return USBDIF_STATUS_SUCCESS;
}




/**
  * @brief  setup standard  GET INTERFACE
  * @details register the USB AUDIO class to USBDIF
  * @param  drc  registered class number
  * @param  s the structure data of setup packet
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioStandardGetInterface(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  usbdAudioHandle_t     *pAudio;

  pAudio = (usbdAudioHandle_t *) prc->pClassData;

  s->len = 1;
  /* get the corresponding value */
  if     (s->wIndex == pAudio->ifCtrl) s->ptr[0] = pAudio->altCtrl;
  else if(s->wIndex == pAudio->ifOut)  s->ptr[0] = pAudio->altOut;
  else if(s->wIndex == pAudio->ifIn)   s->ptr[0] = pAudio->altIn;
  else {
    s->len = 0;
    re = USBDIF_STATUS_STALL;
  }

  return re;
}
/**
  * @brief  setup standard  SET INTERFACE
  * @details register the USB AUDIO class to USBDIF
  * @param  drc  registered class number
  * @param  s the structure data of setup packet
  * @retval result [USBD_OK, FAIL]
  */
static usbdifStatus_t
UsbdAudioStandardSetInterface(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  int                   re = USBDIF_STATUS_SUCCESS;
  usbdAudioHandle_t     *pAudio;
  int                   dev;
  uint8_t               val;

  pAudio = (usbdAudioHandle_t *) prc->pClassData;
  dev = prc->dev;

  val = s->wValue & 0xff;
  if(s->wIndex == pAudio->altCtrl) {
    pAudio->altCtrl = val;

  } else if(s->wIndex == pAudio->ifOut) {
    pAudio->altOut  = val;
    if(val) {
      UsbdevFlush(prc->dev, pAudio->epOut);
      UsbdevFlush(prc->dev, pAudio->epFb);
      UsbdevOpenEp(prc->dev, pAudio->epOut, USBIF_EP_ISOC, pAudio->epSizeOut);
      UsbdevOpenEp(prc->dev, pAudio->epFb, USBIF_EP_ISOC, 4);

      uint8_t       buf[4];
      int           clk = 48<<14;
      buf[0] = (clk >>  0) & 0xff;
      buf[1] = (clk >>  8) & 0xff;
      buf[2] = (clk >> 16) & 0xff;
      buf[3] = (clk >> 24) & 0xff;
      UsbdevTransmit(prc->dev, pAudio->epFb, buf, sizeof(buf));
    } else {
      UsbdevCloseEp(prc->dev, pAudio->epOut);
      UsbdevCloseEp(prc->dev, pAudio->epFb);
    }
  } else if(s->wIndex == pAudio->ifIn) {
    pAudio->altIn   = val;
    if(val) {
      UsbdevFlush(prc->dev, pAudio->epIn);
      UsbdevOpenEp(prc->dev, pAudio->epIn, USBIF_EP_ISOC, pAudio->epSizeIn);
      UsbdevTransmit(prc->dev, pAudio->epIn, usbdaudioZeros, pAudio->pktDefaultSizeIn);
    } else {
      UsbdevCloseEp(prc->dev, pAudio->epIn);
    }
  } else {
    re = USBDIF_STATUS_STALL;
  }

  return re;
}


/**
  * @brief  register the class interface (audio)
  * @details register the USB AUDIO class to USBDIF
  * @param  drc  registered class number
  * @param  fops the pointer to callback function table
  * @retval result [USBD_OK, FAIL]
  */
uint8_t
UsbdAudioRegisterInterface(int drc, usbdAudioCb_t *fops)
{
  usbdifClassDef_t  *prc;
  prc = &usbdif.rc[drc];

  if(fops != NULL) {
    prc->pUserData = fops;
  }
  return 0;
}


/*
 * void UsbdAudioSync(int drc, AUDIO_OffsetTypeDef offset)
 */
#if 0
void
UsbdAudioSync(int drc, AUDIO_OffsetTypeDef offset)
{
  int8_t shift = 0;
  usbdAudioHandle_t   *pAudio;

  usbdifClassDef_t  *prc;
  prc = &usbdif.rc[drc];
  pAudio = (usbdAudioHandle_t *) prc->pClassData;

  pAudio->offset =  offset;

  if(pAudio->rd_enable == 1) {
    pAudio->rd_ptr += AUDIO_TOTAL_BUF_SIZE/2;

    if (pAudio->rd_ptr == AUDIO_TOTAL_BUF_SIZE) {
      /* roll back */
      pAudio->rd_ptr = 0;
    }
  }

  if(pAudio->rd_ptr > pAudio->wr_ptr) {
    if((pAudio->rd_ptr - pAudio->wr_ptr) < AUDIO_INOUT_PACKET_SIZE) {
      shift = -4;
    } else if((pAudio->rd_ptr - pAudio->wr_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_INOUT_PACKET_SIZE)) {
      shift = 4;
    }

  } else {
    if((pAudio->wr_ptr - pAudio->rd_ptr) < AUDIO_INOUT_PACKET_SIZE) {
      shift = 4;
    } else if((pAudio->wr_ptr - pAudio->rd_ptr) > (AUDIO_TOTAL_BUF_SIZE - AUDIO_INOUT_PACKET_SIZE)) {
      shift = -4;
    }
  }

  if(pAudio->offset == AUDIO_OFFSET_FULL) {
    ((usbdAudioCb_t *)prc->pUserData)->audioCmd(&pAudio->buffer[0],
                                                AUDIO_TOTAL_BUF_SIZE/2 - shift,
                                                AUDIO_CMD_PLAY);
    pAudio->offset = AUDIO_OFFSET_NONE;
  }
}
#endif


/**
  * @brief  get the received data information
  * @details get the buffer pointer, the size of buffer (available to continuous copy)
             and the total buffer size
  * @param  drc registered class number
  * @param  ptr the pointer to buffer pointer of the valid data in the rx buffer
  * @param  psz the pointer to the size of buffer (available to continuous copy)
  * @param  pszRecv the pointer to the size of the receviced data (total size)
  * @retval result [USBD_OK, FAIL]
  */
usbdifStatus_t
UsbdAudioGetRecvPointer(int drc, uint8_t **ptr, int *psz, int *pszRecv)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  usbdifClassDef_t      *prc;
  usbdAudioHandle_t     *pAudio;
  int                   size;

  prc = &usbdif.rc[drc];
  pAudio = (usbdAudioHandle_t *) prc->pClassData;
  if(!ptr || prc->type != USBDIF_CLASSTYPE_AUDIO || !pAudio) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  *ptr = &pAudio->outBufPtr[pAudio->outBufPosRd];
  size = pAudio->outBufPosWr - pAudio->outBufPosRd;
  if(size < 0) {
    size += pAudio->outBufSize;
    if(psz) *psz = pAudio->outBufSize - pAudio->outBufPosRd;
  }
  if(pszRecv) *pszRecv = size;

  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}
/**
  * @brief  increase the read position
  * @details get the buffer pointer, the size of buffer (available to continuous copy)
             and the total buffer size
  * @param  drc  registered class number
  * @param  sz   increase size of the read position of the recv buffer
  * @retval result [USBD_OK, FAIL]
  */
usbdifStatus_t
UsbdAudioAddRecvPointer(int drc, int sz)
{
  usbdifStatus_t        result = USBDIF_STATUS_UNKNOWN;
  usbdifClassDef_t      *prc;
  usbdAudioHandle_t     *pAudio;

  prc = &usbdif.rc[drc];
  pAudio = (usbdAudioHandle_t *) prc->pClassData;
  if(prc->type != USBDIF_CLASSTYPE_AUDIO || !pAudio) {
    result = USBDIF_STATUS_INVALIDARG;
    goto fail;
  }

  pAudio->outBufPosRd += sz;
  if(pAudio->outBufPosRd >= pAudio->outBufSize) {
    pAudio->outBufPosRd = pAudio->outBufPosRd - pAudio->outBufSize;
  }
  result = USBDIF_STATUS_SUCCESS;

fail:
  return result;
}

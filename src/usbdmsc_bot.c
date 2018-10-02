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

#define _USBDMSC_BOT_C_

#include        <stdio.h>

#include        "usb_def.h"
#include        "usbdif.h"
#include        "usbdmsc_scsi.h"
#include        "usbdmsc_bot.h"
#include        "usbdmsc.h"

#include        "usb_desc.h"

/*
 *           Host                Target
 * setting:  CBW(31)         ->  CSW(13)
 * data out: CBW(31) + data  ->  CSW(13)
 * data in:  CBW(31)         ->  data + CSW(13)
 *
 */
struct _stUsbmscBot {
  uint8_t               state;
#define USBDMSCBOT_STATE_IDLE         0
#define USBDMSCBOT_STATE_DATA         1
#define USBDMSCBOT_STATE_STATUS       2

  __ALIGN_BEGIN usbdmscBotCbw_t       cbw; __ALIGN_END
  __ALIGN_BEGIN usbdmscBotCsw_t       csw; __ALIGN_END
  uint8_t               maxLun;
};
static struct _stUsbmscBot     bot;


usbdifStatus_t
UsbdmscBotInit(usbdifClassDef_t *prc)
{
  memset(&bot, 0, sizeof(bot));

  return USBDIF_STATUS_SUCCESS;
}


usbdifStatus_t
UsbdmscBotDeinit(usbdifClassDef_t *prc)
{
  memset(&bot, 0, sizeof(bot));

  return USBDIF_STATUS_SUCCESS;
}


usbdifStatus_t
UsbdmscBotSendStatus(usbdifClassDef_t *prc, uint8_t status)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  memcpy(bot.csw.signature, USBDMSC_BOT_SIGNATURE_CSW, USBDMSC_BOT_SIGNATURE_LEN);
  memcpy(bot.csw.tag, bot.cbw.tag, sizeof(bot.csw.tag));
  memset(bot.csw.residue, 0, sizeof(bot.csw.residue));
  bot.csw.status = status;

  UsbdevTransmit(prc->dev, USBDESC_EP_MSC_IN, (uint8_t*)&bot.csw, sizeof(bot.csw));

  return result;
}


static uint8_t  *usbdmscBotSendBuf;
static int      usbdmscBotSendBufLen;
static int      usbdmscBotSendBufPos;
static int      usbdmscBotSendLen;
static int      usbdmscBotSendPos;
                                      /* adhoc   change usbdmscBotSetBuffer */
usbdifStatus_t
UsbdmscBotSendData(usbdifClassDef_t *prc, uint8_t *buf, int szBuf, int szTotal)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  int                   len;
  uint8_t               *ptr;

  usbdmscBotSendBuf = buf;
  usbdmscBotSendBufLen = szBuf;
  usbdmscBotSendBufPos = 0;
  if(szTotal > 0) {
    usbdmscBotSendLen = szTotal;
    usbdmscBotSendPos = 0;
  }

  result = USBDIF_STATUS_SUCCESS;

  return result;
}


usbdifStatus_t
UsbdmscBotDataInStart(usbdifClassDef_t *prc, uint8_t epnum)
{
  int                   size;
  size = usbdmscBotSendLen;
  if(size > 4096) size = 4096;

  UsbdevTransmit(prc->dev, USBDESC_EP_MSC_IN, usbdmscBotSendBuf, size);
  usbdmscBotSendPos += size;
  bot.state = USBDMSCBOT_STATE_DATA;

  return USBDIF_STATUS_SUCCESS;
}
usbdifStatus_t
UsbdmscBotDataInDone(usbdifClassDef_t *prc, uint8_t epnum)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  int                   len;
  uint8_t               *ptr;

  if(bot.state == USBDMSCBOT_STATE_IDLE) {

  } else if(bot.state == USBDMSCBOT_STATE_DATA) {
    if(usbdmscBotSendPos < usbdmscBotSendLen) {
      int       size;
      size = usbdmscBotSendLen - usbdmscBotSendPos;
      if(size > usbdmscBotSendBufLen) size = usbdmscBotSendBufLen;

      UsbdmscScsiDataRead(prc, epnum/*, &ptr, &len*/);
      UsbdevTransmit(prc->dev, USBDESC_EP_MSC_IN, usbdmscBotSendBuf, size);
      usbdmscBotSendPos += size;

    } else {
      UsbdmscBotSendStatus(prc, USBDMSC_BOT_STATUS_SUCCESS);
      usbdmscBotSendLen = 0;
      usbdmscBotSendPos = 0;
      bot.state = USBDMSCBOT_STATE_IDLE;

    }

    result = USBDIF_STATUS_SUCCESS;
  }

  return result;
}

usbdifStatus_t
UsbdmscBotDataOut(usbdifClassDef_t *prc, uint8_t epnum, uint8_t **buf, int *size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdmscBotCbw_t       *cbw;
  uint32_t              dataLen;
  uint8_t               status;

  if(!buf || !*buf) goto fail;

  if(bot.state == USBDMSCBOT_STATE_IDLE) {
    /* check the signature */
    if(memcmp(*buf, USBDMSC_BOT_SIGNATURE_CBW, USBDMSC_BOT_SIGNATURE_LEN)) goto fail;

    memcpy(&bot.cbw, *buf, sizeof(bot.cbw));
    dataLen  =  bot.cbw.dataLen[0];
    dataLen |= (bot.cbw.dataLen[1] <<  8);
    dataLen |= (bot.cbw.dataLen[2] << 16);
    dataLen |= (bot.cbw.dataLen[3] << 24);

    /* scsi command parse */
    status = UsbdmscScsiCtrl(prc, bot.cbw.cb, bot.cbw.cbLen, dataLen);

    /* send the some scsi data, if has    then status send in DataInDone()
     * the status is sent, if no scsi data is sent immediately
     */
    if(dataLen) {
      if(bot.cbw.flags & USBDMSC_BOT_FLAG_DIR_MASK) { /* in */
        UsbdmscBotDataInStart(prc, USBDESC_EP_MSC_IN);
      } else {  /* out */
        usbdmscBotSendLen = dataLen;
        usbdmscBotSendPos = 0;
        *buf  = usbdmscBotSendBuf;
        *size = usbdmscBotSendBufLen;
        bot.state = USBDMSCBOT_STATE_DATA;
      }
    } else {
      UsbdmscBotSendStatus(prc, status);
    }

  } else if(bot.state == USBDMSCBOT_STATE_DATA) {

    /*memcpy(usbdmscBotSendBuf+usbdmscBotSendBufPos, *buf, *size);*/
    usbdmscBotSendBufPos += *size;
    if(usbdmscBotSendBufPos >= usbdmscBotSendBufLen) {
      UsbdmscScsiDataWrite(prc, usbdmscBotSendBuf, usbdmscBotSendBufLen);
      *buf  = usbdmscBotSendBuf;
      *size = usbdmscBotSendBufLen;
    }
    /*((usbdmscCb_t *)prc->pUserData)->write(prc, buf, size);*/
    usbdmscBotSendPos += *size;
    if(usbdmscBotSendPos >= usbdmscBotSendLen) {
      UsbdmscBotSendStatus(prc, USBDIF_STATUS_SUCCESS);
      *buf  = NULL;
      *size = 0;
      bot.state = USBDMSCBOT_STATE_IDLE;
    } else {
    }
  }

  result = USBDIF_STATUS_SUCCESS;
fail:
  return result;
}


usbdifStatus_t
UsbdMscBotSetup(usbdifClassDef_t *prc, usbifSetup_t *s)
{
  usbdifStatus_t        re = USBDIF_STATUS_SUCCESS;
  /*usbdMscHandle_t       *pMsc = (usbdMscHandle_t*) prc->pClassData;*/
  int                   dev;

  dev = prc->dev;
  if((s->bmRequest & USB_BMREQ_TYPE_MASK) == USB_BMREQ_TYPE_CLASS) {

    switch(s->bRequest) {
    case      USBDMSC_BOT_BREQ_GET_MAX_LUN:
      if(s->wLength == 1 && (s->bmRequest & 0x80)) {
        /*UsbdevCtrlSendData(prc->dev, &bot.maxLun, 1);*/
        s->ptr[0] = bot.maxLun;
        s->len = 1;
      } else {
        /*UsbdevCtrlError(prc->dev, s);*/
        re = USBDIF_STATUS_STALL;
      }
      break;
#if 0
    case      USBDMSC_BOT_BREQ_RESET:
      break;
#endif
    default:
      /*UsbdevCtrlError(prc->dev, s);*/
      re = USBDIF_STATUS_STALL;
      break;
    }

  } else {
    re = USBDIF_STATUS_STALL;
  }

  return re;
}

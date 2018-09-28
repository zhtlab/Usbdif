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

#ifndef _USBDMSC_BOT_H_
#define _USBDMSC_BOT_H_

/* bRequest  specified in mass-storage class */
#define USBDMSC_BOT_BREQ_GET_MAX_LUN    0xfe
#define USBDMSC_BOT_BREQ_RESET          0xff


#define USBDMSC_BOT_CBW_SIZE            (31)
#define USBDMSC_BOT_CSW_SIZE            (13)
#define USBDMSC_BOT_SIGNATURE_CBW       "USBC"
#define USBDMSC_BOT_SIGNATURE_CSW       "USBS"
#define USBDMSC_BOT_SIGNATURE_LEN       4


/*** CBW(command block wrapper), CSW(command status wrapper) */
typedef struct _stUsbdmscBotCbw {
  uint8_t       signature[4];
  uint8_t       tag[4];
  uint8_t       dataLen[4];
  uint8_t       flags;
#define USBDMSC_BOT_FLAG_DIR_MASK       (1<<7)  /* payload is H->D */
#define USBDMSC_BOT_FLAG_DIR_OUT        (0<<7)  /* payload is H->D */
#define USBDMSC_BOT_FLAG_DIR_IN         (1<<7)  /* payload is D->H */
  uint8_t       lun;
  uint8_t       cbLen;
  uint8_t       cb[15]; /* scsi command block */
} usbdmscBotCbw_t;
typedef struct _stUsbdmscBotCsw {
  uint8_t       signature[4];
  uint8_t       tag[4];
  uint8_t       residue[4];
  uint8_t       status;
#define USBDMSC_BOT_STATUS_SUCCESS      0
#define USBDMSC_BOT_STATUS_COMMAND_FAIL 1
#define USBDMSC_BOT_STATUS_PAHSE_ERROR  2
} usbdmscBotCsw_t;

/* prototypes */
/* for upper layer (esp. scsi layer) */
usbdifStatus_t          UsbdmscBotSendStatus(usbdifClassDef_t *prc, uint8_t status);
usbdifStatus_t          UsbdmscBotSendData(usbdifClassDef_t *prc, uint8_t *buf, int szBuf, int szTotal);

/* for lower layer */
usbdifStatus_t          UsbdmscBotInit(usbdifClassDef_t *prc);
usbdifStatus_t          UsbdmscBotDeinit(usbdifClassDef_t *prc);
usbdifStatus_t          UsbdmscBotDataInStart(usbdifClassDef_t *prc, uint8_t epnum);
usbdifStatus_t          UsbdmscBotDataInDone(usbdifClassDef_t *prc, uint8_t epnum);
usbdifStatus_t          UsbdmscBotDataOut(usbdifClassDef_t *prc, uint8_t epnum, uint8_t **buf, int *size);
usbdifStatus_t          UsbdMscBotSetup(usbdifClassDef_t *prc, usbifSetup_t *s);


#endif

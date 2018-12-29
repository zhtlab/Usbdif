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

#ifndef _USBDHID_H_
#define _USBDHID_H_

#define HID_DATA_HS_MAX_PACKET_SIZE             512
#define HID_DATA_FS_MAX_PACKET_SIZE             64

#define USBDHID_PACKET_SIZE_FS                  64
#define USBDHID_PACKET_SIZE_HS                  64

typedef struct {
  int           (*init)(usbdifClassDef_t *prc);                                 /* adhoc */
  int           (*deinit)(usbdifClassDef_t *prc);                               /* adhoc */
  int           (*ctrl)(usbdifClassDef_t *prc, uint8_t req, uint8_t *ptr, int size);
#define USBDHID_CB_REQ_GET_REPORT0              0x10
#define USBDHID_CB_REQ_GET_REPORT1              0x11
#define USBDHID_CB_REQ_GET_REPORT2              0x12
#define USBDHID_CB_REQ_GET_REPORT3              0x13
#define USBDHID_CB_REQ_GET_REPORT(x)            (0x10 | ((x) & 0xf))
  int           (*recv)(usbdifClassDef_t *prc, uint8_t *ptr, int *len);
  int           (*send)(usbdifClassDef_t *prc, uint8_t *ptr, int *len);
  void          *pHidParam;
} usbdhidCb_t;


int                     UsbdHidIsTxBusy(int rcnum);
usbdifStatus_t          UsbdHidTransmit(int rcnum, uint8_t *ptr, uint16_t len);
usbdifStatus_t          UsbdHidReceiving(int rcnum, uint8_t *ptr, uint16_t len);


#ifdef  _USBDHID_C_

typedef struct
{
  uint32_t              enOutputReport :1;
  uint8_t               *RxBuffer;
  uint8_t               *TxBuffer;
  uint32_t              RxLength;
  uint32_t              TxLength;

  /* end point informations */
  uint8_t               epIn;
  uint8_t               epOut;

  /* idle/busy state */
  volatile uint8_t      TxState;
  volatile uint8_t      RxState;

  rtosSemId             idSemSend;

} usbdHidHandle_t;


static usbdifStatus_t  UsbdHidInit(usbdifClassDef_t *prc, int cfgnum);
static usbdifStatus_t  UsbdHidDeInit(usbdifClassDef_t *prc, int cfgnum);
static usbdifStatus_t  UsbdHidSetup(usbdifClassDef_t *prc, usbifSetup_t *req);
static usbdifStatus_t  UsbdHidDataInDone(usbdifClassDef_t *prc, uint8_t epnum);
static usbdifStatus_t  UsbdHidDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size);
static usbdifStatus_t  UsbdHidEP0_RxReady(usbdifClassDef_t *prc);

#endif

#endif

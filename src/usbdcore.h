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

#ifndef _USBDCORE_H_
#define _USBDCORE_H_



usbdifStatus_t          UsbdcoreInit(void);

usbdifStatus_t          UsbdcoreCbIsr(int unit, uint8_t req, uint8_t num, void *p);
enum {
  USBDCORECB_REQ_BUSSTATE = 0,
  USBDCORECB_REQ_SETUP = 1,
  USBDCORECB_REQ_DATAOUT = 2,
  USBDCORECB_REQ_DATAINDONE = 3,
};
typedef struct {
  uint8_t       unit;
  uint8_t       req;
  uint8_t       num;
  uint8_t       reserved;
  void          *p;
} usbdcoreCbPkt;
usbdifStatus_t          UsbdcoreCbExecute(void);

usbdifStatus_t          UsbdcoreCbBusState(int dev, usbdifBusState_t state);
usbdifStatus_t          UsbdcoreCbSetup(int unit, usbifSetup_t *s);
usbdifStatus_t          UsbdcoreCbDataOut(int unit, uint8_t epnum, int size);
usbdifStatus_t          UsbdcoreCbDataInDone(int unit, uint8_t epnum);

usbdifStatus_t          UsbdevCtrlSendData(int unit, uint8_t *buf, uint16_t len);
usbdifStatus_t          UsbdevCtrlStatus(int unit);
void                    UsbdevCtrlError(int unit, usbifSetup_t *s);


usbdifStatus_t          UsbdevOpenEp(int unit, uint8_t epnum, uint8_t epType, uint16_t epMps);
usbdifStatus_t          UsbdevCloseEp(int unit, uint8_t epnum);
usbdifStatus_t          UsbdevPrepareRecv(int unit, uint8_t epnum, uint8_t *pbuf, int size);
int                     UsbdevGetRxDataSize(int unit, uint8_t epnum);
usbdifStatus_t          UsbdevTransmit(int unit, uint8_t epnum, const uint8_t *pbuf, int size);
usbdifStatus_t          UsbdevFlush(int dev, uint8_t epnum);
void                    UsbdevSofEntry(int unit);


#ifdef  _USBDCORE_C_
static uint8_t          UsbdcoreSetupStandardDeviceRequest(struct _stUsbdifDev *psc, usbifSetup_t *s);
static int              UsbdcoreBuildStringDesc(uint8_t *str);

static void             UsbdevCtrlStall(int unit);


/* the function prototype of the lower module(chip) device driver */
int             DevUsbOpenEp(int unit, uint8_t epnum, int type, int size);
int             DevUsbCloseEp(int unit, uint8_t epnum);
int             DevUsbTransmit(int unit, uint8_t epnum, const uint8_t *ptr, int size);
int             DevUsbPrepareReceive(int unit, uint8_t epnum, const uint8_t *ptr, int size);
int             DevUsbSetStall(int unit, uint8_t epnum);
int             DevUsbSetAddress(int unit, int address);

#endif

#endif

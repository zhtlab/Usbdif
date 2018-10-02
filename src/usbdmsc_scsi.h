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

#ifndef _USBDMSC_SCSI_H_
#define _USBDMSC_SCSI_H_

#define USBDMSC_SCSI_CMD_TESTUNITREADY  0x00
#define USBDMSC_SCSI_CMD_REQUESTSENSE   0x03
#define USBDMSC_SCSI_CMD_INQUIRY        0x12
#define USBDMSC_SCSI_CMD_MODE_SENSE6    0x1a
#define USBDMSC_SCSI_CMD_START_STOP_UNIT  0x1b
#define USBDMSC_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL   0x1e
#define USBDMSC_SCSI_CMD_READCAPACITY   0x25
#define USBDMSC_SCSI_CMD_READ10         0x28
#define USBDMSC_SCSI_CMD_WRITE10        0x2A

#define USBDMSC_SCSI_INQUIRY_VENDER_LEN         8
#define USBDMSC_SCSI_INQUIRY_PRODUCT_LEN        16
#define USBDMSC_SCSI_INQUIRY_VERSION_LEN        4
typedef struct {
  uint8_t       vendor[USBDMSC_SCSI_INQUIRY_VENDER_LEN];
  uint8_t       product[USBDMSC_SCSI_INQUIRY_PRODUCT_LEN];
  uint8_t       version[USBDMSC_SCSI_INQUIRY_VERSION_LEN];
} usbdmscScsiVendorProduct;

typedef struct {
  uint32_t      maxLba;
  uint16_t      blockSize;
  uint8_t       ready;
  uint8_t       allowRemoval;
} usbdmscScsiDriveInfo_t;



usbdifStatus_t          UsbdmscScsiCtrl(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);


usbdifStatus_t          UsbdmscScsiDataWrite(usbdifClassDef_t *prc, uint8_t *buf, int size);
usbdifStatus_t          UsbdmscScsiDataRead(usbdifClassDef_t *prc, uint8_t epnum);

#ifdef _USBDMSC_SCSI_C_
static usbdifStatus_t   UsbdmscScsiCommandInquiry(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandRead10(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandWrite10(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandRequestSense(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandTestUnitReady(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandReadCapacity(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandPreventAllowMediumRemoval(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandModeSense(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);
static usbdifStatus_t   UsbdmscScsiCommandStartStopUnit(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen);

static usbdifStatus_t   UsbdmscScsiDataReadAndSend(usbdifClassDef_t *prc, int fFirst);
static usbdifStatus_t   UsbdmscScsiDataRecvAndWrite(usbdifClassDef_t *prc, int fFirst, uint8_t *buf, int size);

#endif

#endif

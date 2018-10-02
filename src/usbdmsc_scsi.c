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

#define _USBDMSC_SCSI_C_

#include        "stdio.h"

#include        "usb_def.h"
#include        "usbdif.h"
#include        "usb_desc.h"

#include        "usbdmsc_bot.h"
#include        "usbdmsc.h"

#include        "usbdmsc_scsi.h"

#define USBDMSC_SCSI_DEBUG_SHOW_COMMAND 0
#define USBDMSC_SCSI_DEBUG_SHOW_UNSUPPORTED_COMMAND 1

int             sizeTx;
uint8_t         bufTx[256];             /* local buffer */
int8_t          status;


usbdifStatus_t
UsbdmscScsiCtrl(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  uint8_t               op, lun, len;

  op  = buf[0];
  lun = buf[1] >> 5;
  len = buf[4];

  /*printf("XXXX scsi %x %x %x --------------\r\n", op, lun, len);*/


  /*((usbdmscCb_t *)prc->pUserData)->ctrl(lun, 0, buf, size);*/


  switch(op) {
  case USBDMSC_SCSI_CMD_READ10:                 /* 0x28 */
    result = UsbdmscScsiCommandRead10(prc, buf, size, respLen);
    break;
  case USBDMSC_SCSI_CMD_WRITE10:                /* 0x2a */
    result = UsbdmscScsiCommandWrite10(prc, buf, size, respLen);
    break;

  case USBDMSC_SCSI_CMD_TESTUNITREADY:          /* 0x00 */
    result = UsbdmscScsiCommandTestUnitReady(prc, buf, size, respLen);
    break;

  case USBDMSC_SCSI_CMD_REQUESTSENSE:           /* 0x03 */
    result = UsbdmscScsiCommandRequestSense(prc, buf, size, respLen);
    break;
  case USBDMSC_SCSI_CMD_READCAPACITY:           /* 0x25 */
    result = UsbdmscScsiCommandReadCapacity(prc, buf, size, respLen);
    break;
  case USBDMSC_SCSI_CMD_INQUIRY:                /* 0x12 */
    result = UsbdmscScsiCommandInquiry(prc, buf, size, respLen);
    break;
  case        USBDMSC_SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL:    /* 0x1e */
    result = UsbdmscScsiCommandPreventAllowMediumRemoval(prc, buf, size, respLen);
    break;
  case        USBDMSC_SCSI_CMD_MODE_SENSE6:     /* 0x1a */
    result = UsbdmscScsiCommandModeSense(prc, buf, size, respLen);
    break;
  case        USBDMSC_SCSI_CMD_START_STOP_UNIT: /* 0x1b */
    result = UsbdmscScsiCommandStartStopUnit(prc, buf, size, respLen);
    break;
  default:
#if (USBDMSC_SCSI_DEBUG_SHOW_COMMAND || USBDMSC_SCSI_DEBUG_SHOW_UNSUPPORTED_COMMAND)
    printf("MSC:SCSI unsupported command %x\r\n", op);
#endif
    result = 1;
  };





  return result;
}



static usbdifStatus_t
UsbdmscScsiCommandInquiry(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  usbdmscScsiVendorProduct      product;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI inquiry\r\n");
#endif

  ((usbdmscCb_t *)prc->pUserData)->ctrl(0,
                                        USBDMSC_CTRL_GET_SCSI_VENDORPRODUCT,
                                        (uint8_t *)&product, sizeof(product));

  sizeTx = 0x24;
  memset(bufTx,     0, 8);
  memset(bufTx+8, ' ', 0x24-8);
  bufTx[0] = 0x00;      /* [7:5] peripheral qualifier, [4/0] device type */
  bufTx[1] = 0x80;      /* [7]   removable */
  bufTx[2] = 0x06;      /* [7:0] version */
  bufTx[3] = 0x02;      /* [5]   normaca, [4] hisup, [3:0] data format */
  bufTx[4] = 0x1f;      /* [7:0] additional length */
  bufTx[5] = 0x00;      /* [7] sccs, [6] acc, [5:4] tpgs, [3] 3pc, [0] protect */
  bufTx[6] = 0x00;      /* [6] encserv, [5] vs, [4] multip, [3] mchngr, [0] addr16 */
  bufTx[7] = 0x00;      /* [5] wbus16, [4] sync, [1] cmdque, [0] vs */

  memcpy(&bufTx[8],  product.vendor, USBDMSC_SCSI_INQUIRY_VENDER_LEN);
  memcpy(&bufTx[16], product.product, USBDMSC_SCSI_INQUIRY_PRODUCT_LEN);
  memcpy(&bufTx[32], product.version, USBDMSC_SCSI_INQUIRY_VERSION_LEN);

  UsbdmscBotSendData(prc, bufTx, sizeTx, sizeTx);

  result = 0;

  return result;
}


static uint8_t         lun;
static uint32_t        lba;
static uint16_t        lbaCnt;
static usbdmscScsiDriveInfo_t driveInfo[8];
static usbdifStatus_t
UsbdmscScsiCommandRead10(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  uint8_t               *ptr;
  int                   len;
  uint16_t              cnt;

  lun  = buf[1] >>  5;
  lba  = buf[2] << 24;
  lba |= buf[3] << 16;
  lba |= buf[4] <<  8;
  lba |= buf[5];
  lbaCnt  = buf[7] << 8;
  lbaCnt |= buf[8];

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI read(10) lun:%d lba:%x cnt:%x\r\n",
         lun, lba, lbaCnt);
#endif

  result = UsbdmscScsiDataReadAndSend(prc, 1);

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandWrite10(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  uint8_t               *ptr;
  int                   len;
  int                   cnt;

  lun  = buf[1] >>  5;
  lba  = buf[2] << 24;
  lba |= buf[3] << 16;
  lba |= buf[4] <<  8;
  lba |= buf[5];
  lbaCnt  = buf[7] << 8;
  lbaCnt |= buf[8];

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI write(10) lun:%d lba:%x cnt:%x\r\n",
         lun, lba, lbaCnt);
#endif

  result = UsbdmscScsiDataRecvAndWrite(prc, 0, NULL, 0);

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandRequestSense(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI request sense\r\n");
#endif

  sizeTx = 0x18;
  memset(bufTx,     0, 0x18);
  bufTx[0] = 0x07;      /* [7:5] peripheral qualifier, [4/0] device type */
  bufTx[1] = 0x80;      /* [7]   removable */
  bufTx[2] = 0x05;      /* [7:0] versin */
  bufTx[3] = 0x00;      /* [5]   normaca, [4] hisup, [3:0] data format */
  bufTx[4] = 0x00;      /* [7:0] additional length */
  bufTx[5] = 0x00;      /* [7] sccs, [6] acc, [5:4] tpgs, [3] 3pc, [0] protect */
  bufTx[6] = 0x00;      /* [6] encserv, [5] vs, [4] multip, [3] mchngr, [0] addr16 */
  bufTx[7] = 0x0a;      /* additional sense length */
  bufTx[8] = 0x00;      /* command specification information */
  bufTx[12] = 0x24;     /* additional sense code  0x24/0x28 */
  UsbdmscBotSendData(prc, bufTx, sizeTx, sizeTx);

  result = 0;

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandTestUnitReady(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;
  int                   ready;
  int                   lun;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI test unit ready\r\n");
#endif
  lun = 0;
  ((usbdmscCb_t *)prc->pUserData)->ctrl(lun, USBDMSC_CTRL_GET_SCSI_DRIVEINFO,
                                        (void *)&driveInfo[lun], sizeof(usbdmscScsiDriveInfo_t));
  result = (driveInfo[lun].ready)? 0: 1;

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandReadCapacity(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  uint32_t              lba;
  int                   lun;
  usbdmscScsiDriveInfo_t        *p;


#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI read capacity\r\n");
#endif

  lun = 0;

  p = &driveInfo[lun];
  ((usbdmscCb_t *)prc->pUserData)->ctrl(lun, USBDMSC_CTRL_GET_SCSI_CAPABILITY,
    (void *)p, sizeof(usbdmscScsiDriveInfo_t));
  bufTx[0] = p->maxLba >> 24;
  bufTx[1] = p->maxLba >> 16;
  bufTx[2] = p->maxLba >>  8;
  bufTx[3] = p->maxLba;
  bufTx[4] = p->blockSize >> 24;
  bufTx[5] = p->blockSize >> 16;
  bufTx[6] = p->blockSize >>  8;
  bufTx[7] = p->blockSize;

  UsbdmscBotSendData(prc, bufTx, 8, 8);

  result = 0;

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandPreventAllowMediumRemoval(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result;
  int                   lun;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI prevent allow medium Removal %x %x\r\n", buf[4], buf[5]);
#endif

  lun = 0;
  driveInfo[lun].allowRemoval = buf[4];
  ((usbdmscCb_t *)prc->pUserData)->ctrl(lun, USBDMSC_CTRL_SET_SCSI_DRIVEINFO,
                                        (void *)&driveInfo[lun], sizeof(usbdmscScsiDriveInfo_t));

  result = 0;

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandModeSense(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI mode sense\r\n");
#endif

  memset(bufTx, 0, 0xc0);
  bufTx[0] = 0x23;
  bufTx[4] = 0x05;
  bufTx[5] = 0x1e;
  bufTx[6] = 0xf0;
  bufTx[8] = 0xff;
  bufTx[9] = 0x20;
  bufTx[10] = 0x02;
  bufTx[12] = 0x07;
  bufTx[13] = 0x47;
  UsbdmscBotSendData(prc, bufTx, 36, 36);

  result = 0;

  return result;
}
static usbdifStatus_t
UsbdmscScsiCommandStartStopUnit(usbdifClassDef_t *prc, uint8_t *buf, int size, int respLen)
{
  int                   result;

#if USBDMSC_SCSI_DEBUG_SHOW_COMMAND
  printf("MSC:SCSI start stop unit\r\n");
#endif

  printf("startstop %x\r\n", buf[4]);

  result = 0;

  return result;
}

usbdifStatus_t
UsbdmscScsiDataWrite(usbdifClassDef_t *prc, uint8_t *buf, int size)
{
  usbdifStatus_t                result;
  uint16_t                      cnt;
  uint8_t                       *ptr;
  int                           len;

  result = UsbdmscScsiDataRecvAndWrite(prc, 0, buf, size);

  return result;
}
usbdifStatus_t
UsbdmscScsiDataRead(usbdifClassDef_t *prc, uint8_t epnum)
{
  usbdifStatus_t        re;

  re = UsbdmscScsiDataReadAndSend(prc, 0);

  return re;
}


usbdifStatus_t
UsbdmscScsiDataReadAndSend(usbdifClassDef_t *prc, int fFirst)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  uint8_t               *ptr;
  int                   len;
  uint16_t              cnt;

  cnt = lbaCnt;
  ((usbdmscCb_t *)prc->pUserData)->read(lun, lba, &cnt, &ptr, &len);
  UsbdmscBotSendData(prc, ptr, len, ((fFirst)? lbaCnt*driveInfo[lun].blockSize: -1));
  lba    += cnt;
  lbaCnt -= cnt;

  result = USBDIF_STATUS_SUCCESS;

  return result;
}


usbdifStatus_t
UsbdmscScsiDataRecvAndWrite(usbdifClassDef_t *prc, int fFirst, uint8_t *buf, int size)
{
  int                   result = USBDIF_STATUS_UNKNOWN;

  uint8_t               *ptr;
  int                   len;
  uint16_t              cnt;

  cnt = lbaCnt;
  ptr = buf;
  len = size;

  ((usbdmscCb_t *)prc->pUserData)->write(lun, lba, &cnt, &ptr, &len);
  UsbdmscBotSendData(prc, ptr, len, -1);
  lba += cnt;

  result = USBDIF_STATUS_SUCCESS;

  return result;
}

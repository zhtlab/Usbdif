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

#ifndef _USBDMSC_H_
#define _USBDMSC_H_


/* MSC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define MSC_DATA_HS_MAX_PACKET_SIZE             512
#define MSC_DATA_FS_MAX_PACKET_SIZE             64

#define USBDMSC_PACKET_SIZE_FS                  64
#define USBDMSC_PACKET_SIZE_HS                  64


#define USBDMSC_PACKET_SIZE_CMD                 8
#define USBDMSC_PACKET_SIZE_FS_IN               (USBDMSC_PACKET_SIZE_FS)
#define USBDMSC_PACKET_SIZE_FS_OUT              (USBDMSC_PACKET_SIZE_FS)
#define USBDMSC_PACKET_SIZE_HS_IN               (USBDMSC_PACKET_SIZE_HS)
#define USBDMSC_PACKET_SIZE_HS_OUT              (USBDMSC_PACKET_SIZE_HS)

#if 0
#define MSC_DATA_HS_IN_PACKET_SIZE              MSC_DATA_HS_MAX_PACKET_SIZE
#define MSC_DATA_HS_OUT_PACKET_SIZE             MSC_DATA_HS_MAX_PACKET_SIZE

#define MSC_DATA_FS_IN_PACKET_SIZE              MSC_DATA_FS_MAX_PACKET_SIZE
#define MSC_DATA_FS_OUT_PACKET_SIZE             MSC_DATA_FS_MAX_PACKET_SIZE
#endif

#define USBDMSC_CTRL_GET_SCSI_VENDORPRODUCT     0x11
#define USBDMSC_CTRL_GET_SCSI_CAPABILITY        0x13
#define USBDMSC_CTRL_SET_SCSI_DRIVEINFO         0x20
#define USBDMSC_CTRL_GET_SCSI_DRIVEINFO         0x21

/*---------------------------------------------------------------------*/
/*  MSC definitions                                                    */
/*---------------------------------------------------------------------*/


typedef struct {
  int           (*init)(usbdifClassDef_t *prc);                                 /* adhoc */
  int           (*deinit)(usbdifClassDef_t *prc);                               /* adhoc */
  int           (*ctrl)(int lun, uint8_t req, uint8_t *ptr, int size);
  int           (*read)(int lun, uint32_t lba, uint16_t *cnt, uint8_t **ptr, int *len);
  int           (*write)(int lun, uint32_t lba, uint16_t *cnt, uint8_t **ptr, int *len);
  void          *pMscParam;
} usbdmscCb_t;



int             UsbdMscIsTxBusy(int rcnum);
usbdifStatus_t  UsbdMscReceiving(int rcnum, uint8_t *ptr, uint16_t len);
usbdifStatus_t  UsbdMscTransmit(int rcnum, uint8_t *ptr, uint16_t len);


#ifdef  _USBDMSC_C_

typedef struct
{
  /* it must put data array on 32bits alignment */
  uint32_t              data[MSC_DATA_HS_MAX_PACKET_SIZE/sizeof(uint32_t)];
  uint8_t               CmdOpCode;
  uint8_t               CmdLength;
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

} usbdMscHandle_t;


static usbdifStatus_t  UsbdMscInit(usbdifClassDef_t *prc, uint8_t cfgidx);
static usbdifStatus_t  UsbdMscDeInit(usbdifClassDef_t *prc, uint8_t cfgidx);
static usbdifStatus_t  UsbdMscSetup(usbdifClassDef_t *prc, usbifSetup_t *req);
static usbdifStatus_t  UsbdMscDataInDone(usbdifClassDef_t *prc, uint8_t epnum);
static usbdifStatus_t  UsbdMscDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size);
static usbdifStatus_t  UsbdMscEP0_RxReady(usbdifClassDef_t *prc);

#endif


#endif

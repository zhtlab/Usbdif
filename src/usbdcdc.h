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

#ifndef _USBDCDC_H_
#define _USBDCDC_H_


/* CDC Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define CDC_DATA_HS_MAX_PACKET_SIZE             512
#define CDC_DATA_FS_MAX_PACKET_SIZE             64

#define USBDCDC_PACKET_SIZE_FS                  64
#define USBDCDC_PACKET_SIZE_HS                  64


#define USBDCDC_PACKET_SIZE_CMD                 8
#define USBDCDC_PACKET_SIZE_FS_IN               (USBDCDC_PACKET_SIZE_FS)
#define USBDCDC_PACKET_SIZE_FS_OUT              (USBDCDC_PACKET_SIZE_FS)
#define USBDCDC_PACKET_SIZE_HS_IN               (USBDCDC_PACKET_SIZE_HS)
#define USBDCDC_PACKET_SIZE_HS_OUT              (USBDCDC_PACKET_SIZE_HS)

#if 0
#define CDC_DATA_HS_IN_PACKET_SIZE              CDC_DATA_HS_MAX_PACKET_SIZE
#define CDC_DATA_HS_OUT_PACKET_SIZE             CDC_DATA_HS_MAX_PACKET_SIZE

#define CDC_DATA_FS_IN_PACKET_SIZE              CDC_DATA_FS_MAX_PACKET_SIZE
#define CDC_DATA_FS_OUT_PACKET_SIZE             CDC_DATA_FS_MAX_PACKET_SIZE
#endif

/*---------------------------------------------------------------------*/
/*  CDC definitions                                                    */
/*---------------------------------------------------------------------*/
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23


#if 0
typedef struct {
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} USBD_CDC_LineCodingTypeDef;
#endif


typedef struct {
  int           (*init)(usbdifClassDef_t *prc);
  int           (*deinit)(usbdifClassDef_t *prc);
  int           (*ctrl)(usbdifClassDef_t *prc, uint8_t, uint8_t *, uint16_t);
  int           (*recv)(usbdifClassDef_t *prc, uint8_t *, int);
  void          *pCdcParam;
} usbdcdcCb_t;



int             UsbdCdcIsTxBusy(int rcnum);
usbdifStatus_t  UsbdCdcReceiving(int rcnum, uint8_t *ptr, uint16_t len);
usbdifStatus_t  UsbdCdcTransmit(int rcnum, uint8_t *ptr, uint16_t len);


#ifdef  _USBDCDC_C_

typedef struct
{
  /* it must put data array on 32bits alignment */
  uint32_t              data[CDC_DATA_HS_MAX_PACKET_SIZE/sizeof(uint32_t)];
  uint8_t               CmdOpCode;
  uint8_t               CmdLength;
  uint8_t               *RxBuffer;
  uint8_t               *TxBuffer;
  uint32_t              RxLength;
  uint32_t              TxLength;

  /* end point informations */
  uint8_t               epIn;
  uint8_t               epOut;
  uint8_t               epCtrl;

  /* idle/busy state */
  volatile uint8_t      TxState;
  volatile uint8_t      RxState;

  rtosSemId             idSemSend;

} usbdCdcHandle_t;


static usbdifStatus_t  UsbdCdcInit(usbdifClassDef_t *prc, int cfgnum);
static usbdifStatus_t  UsbdCdcDeInit(usbdifClassDef_t *prc, int cfgnum);
static usbdifStatus_t  UsbdCdcSetup(usbdifClassDef_t *prc, usbifSetup_t *req);
static usbdifStatus_t  UsbdCdcDataIn(usbdifClassDef_t *prc, uint8_t epnum);
static usbdifStatus_t  UsbdCdcDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size);
static usbdifStatus_t  UsbdCdcEP0_RxReady(usbdifClassDef_t *prc);

#endif


#endif

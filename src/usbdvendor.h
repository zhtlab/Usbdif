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

#ifndef _USBDVENDOR_H_
#define _USBDVENDOR_H_


/* VENDOR Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define VENDOR_DATA_HS_MAX_PACKET_SIZE          512
#define VENDOR_DATA_FS_MAX_PACKET_SIZE          64

#define USBDVENDOR_PACKET_SIZE_FS               64
#define USBDVENDOR_PACKET_SIZE_HS               64


#define USBDVENDOR_PACKET_SIZE_CMD              8
#define USBDVENDOR_PACKET_SIZE_FS_IN            (USBDVENDOR_PACKET_SIZE_FS)
#define USBDVENDOR_PACKET_SIZE_FS_OUT           (USBDVENDOR_PACKET_SIZE_FS)
#define USBDVENDOR_PACKET_SIZE_HS_IN            (USBDVENDOR_PACKET_SIZE_HS)
#define USBDVENDOR_PACKET_SIZE_HS_OUT           (USBDVENDOR_PACKET_SIZE_HS)

#if 0
#define VENDOR_DATA_HS_IN_PACKET_SIZE           VENDOR_DATA_HS_MAX_PACKET_SIZE
#define VENDOR_DATA_HS_OUT_PACKET_SIZE          VENDOR_DATA_HS_MAX_PACKET_SIZE

#define VENDOR_DATA_FS_IN_PACKET_SIZE           VENDOR_DATA_FS_MAX_PACKET_SIZE
#define VENDOR_DATA_FS_OUT_PACKET_SIZE          VENDOR_DATA_FS_MAX_PACKET_SIZE
#endif

/*---------------------------------------------------------------------*/
/*  VENDOR definitions                                                    */
/*---------------------------------------------------------------------*/

#define I2C_FUNC_I2C                    (1 << 0)
#define I2C_FUNC_10BIT_ADDR             (1 << 1)
#define I2C_FUNC_PROTOCOL_MANGLING      (1 << 2) /* I2C_M_{REV_DIR_ADDR,NOSTART,..} */
/* SMBus 2.0 */
#define I2C_FUNC_SMBUS_HWPEC_CALC       (1 << 3)
#define I2C_FUNC_SMBUS_READ_WORD_DATA_PEC  (1 << 11)
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA_PEC (1 << 12)
#define I2C_FUNC_SMBUS_PROC_CALL_PEC       (1 << 13)
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL_PEC (1 << 14)
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL     (1 << 15)

#define I2C_FUNC_SMBUS_QUICK            (1 << 16)
#define I2C_FUNC_SMBUS_READ_BYTE        (1 << 17)
#define I2C_FUNC_SMBUS_WRITE_BYTE       (1 << 18)
#define I2C_FUNC_SMBUS_READ_BYTE_DATA   (1 << 19)
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA  (1 << 20)
#define I2C_FUNC_SMBUS_READ_WORD_DATA   (1 << 21)
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA  (1 << 22)
#define I2C_FUNC_SMBUS_PROC_CALL        (1 << 23)
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA  (1 << 24)
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA (1 << 25)
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK   (1 << 26)/* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK  (1 << 27) /* w/ 1-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK_2 (1 << 28) /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK_2    (1 << 29) /* w/ 2-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA_PEC  (1 << 30) /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC (1 << 31) /* SMBus 2.0 */

#define I2C_FUNC_SMBUS_BYTE I2C_FUNC_SMBUS_READ_BYTE | \
                            I2C_FUNC_SMBUS_WRITE_BYTE
#define I2C_FUNC_SMBUS_BYTE_DATA I2C_FUNC_SMBUS_READ_BYTE_DATA | \
                                 I2C_FUNC_SMBUS_WRITE_BYTE_DATA
#define I2C_FUNC_SMBUS_WORD_DATA I2C_FUNC_SMBUS_READ_WORD_DATA | \
                                 I2C_FUNC_SMBUS_WRITE_WORD_DATA
#define I2C_FUNC_SMBUS_BLOCK_DATA I2C_FUNC_SMBUS_READ_BLOCK_DATA | \
                                  I2C_FUNC_SMBUS_WRITE_BLOCK_DATA
#define I2C_FUNC_SMBUS_I2C_BLOCK I2C_FUNC_SMBUS_READ_I2C_BLOCK | \
                                  I2C_FUNC_SMBUS_WRITE_I2C_BLOCK

#define I2C_FUNC_SMBUS_EMUL I2C_FUNC_SMBUS_QUICK |      \
                            I2C_FUNC_SMBUS_BYTE | \
                            I2C_FUNC_SMBUS_BYTE_DATA | \
                            I2C_FUNC_SMBUS_WORD_DATA | \
                            I2C_FUNC_SMBUS_PROC_CALL | \
                            I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
                            I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC | \
                            I2C_FUNC_SMBUS_I2C_BLOCK



typedef struct {
  int           (*init)(usbdifClassDef_t *prc);
  int           (*deinit)(usbdifClassDef_t *prc);
  int           (*ctrl)(usbdifClassDef_t *prc, usbifSetup_t *s);
  int           (*recv)(usbdifClassDef_t *prc, uint8_t *, int);
  void          *pVendorParam;
} usbdvendorCb_t;



int             UsbdVendorIsTxBusy(int rcnum);
usbdifStatus_t  UsbdVendorReceiving(int rcnum, uint8_t *ptr, uint16_t len);
usbdifStatus_t  UsbdVendorTransmit(int rcnum, uint8_t *ptr, uint16_t len);


#ifdef  _USBDVENDOR_C_

typedef struct
{
  /* it must put data array on 32bits alignment */
  uint32_t              data[VENDOR_DATA_HS_MAX_PACKET_SIZE/sizeof(uint32_t)];
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

} usbdVendorHandle_t;


static usbdifStatus_t  UsbdVendorInit(usbdifClassDef_t *prc, uint8_t cfgidx);
static usbdifStatus_t  UsbdVendorDeInit(usbdifClassDef_t *prc, uint8_t cfgidx);
static usbdifStatus_t  UsbdVendorSetup(usbdifClassDef_t *prc, usbifSetup_t *req);
static usbdifStatus_t  UsbdVendorDataIn(usbdifClassDef_t *prc, uint8_t epnum);
static usbdifStatus_t  UsbdVendorDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size);
static usbdifStatus_t  UsbdVendorEP0_RxReady(usbdifClassDef_t *prc);

#endif


#endif

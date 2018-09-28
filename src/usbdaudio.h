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

#ifndef _USBDAUDIO_H_
#define _USBDAUDIO_H_


#define USBDAUDIO_EP_OUT                                (USBDESC_EP_AUDIO_OUT)
#define USBDAUDIO_EP_IN                                 (USBDESC_EP_AUDIO_IN)
#define USBDAUDIO_EP_FB                                 (USBDESC_EP_AUDIO_FB)

#define USB_AUDIO_CONFIG_DESC_SIZ                     109
#define AUDIO_INTERFACE_DESC_SIZE                     9
#define USB_AUDIO_DESC_SIZ                            0x09
#define AUDIO_STANDARD_ENDPOINT_DESC_SIZE             0x09
#define AUDIO_STREAMING_ENDPOINT_DESC_SIZE            0x07

#define AUDIO_DESCRIPTOR_TYPE                         0x21
#define USB_DEVICE_CLASS_AUDIO                        0x01
#define AUDIO_SUBCLASS_AUDIOCONTROL                   0x01
#define AUDIO_SUBCLASS_AUDIOSTREAMING                 0x02
#define AUDIO_PROTOCOL_UNDEFINED                      0x00
#define AUDIO_STREAMING_GENERAL                       0x01
#define AUDIO_STREAMING_FORMAT_TYPE                   0x02

/* Audio Descriptor Types */
#define AUDIO_INTERFACE_DESCRIPTOR_TYPE               0x24
#define AUDIO_ENDPOINT_DESCRIPTOR_TYPE                0x25

/* Audio Control Interface Descriptor Subtypes */
#define AUDIO_CONTROL_HEADER                          0x01
#define AUDIO_CONTROL_INPUT_TERMINAL                  0x02
#define AUDIO_CONTROL_OUTPUT_TERMINAL                 0x03
#define AUDIO_CONTROL_FEATURE_UNIT                    0x06

#define AUDIO_INPUT_TERMINAL_DESC_SIZE                0x0C
#define AUDIO_OUTPUT_TERMINAL_DESC_SIZE               0x09
#define AUDIO_STREAMING_INTERFACE_DESC_SIZE           0x07

#define AUDIO_CONTROL_MUTE                            0x0001

#define AUDIO_FORMAT_TYPE_I                           0x01
#define AUDIO_FORMAT_TYPE_III                         0x03

#define AUDIO_ENDPOINT_GENERAL                        0x01

#define AUDIO_REQ_GET_CUR                             0x81
#define AUDIO_REQ_SET_CUR                             0x01

#define AUDIO_OUT_STREAMING_CTRL                      0x02


/*#define AUDIO_OUT_PACKET                              (uint32_t)(((CONFIG_AUDIO_FS_48K000 * 2 * 2) /1000)) */
#define AUDIO_INOUT_PACKET_SIZE                       (uint32_t)(((CONFIG_AUDIO_FS_48K000 * 2 * 2) /1000))
#define AUDIO_DEFAULT_VOLUME                          70
/* Number of sub-packets in the audio transfer buffer. You can modify this value but always make sure
  that it is an even number and higher than 3 */
#define AUDIO_OUT_PACKET_NUM                          80
/* Total size of the audio transfer buffer */
#define AUDIO_TOTAL_BUF_SIZE                          ((uint32_t)(AUDIO_INOUT_PACKET_SIZE * AUDIO_OUT_PACKET_NUM))
    /* Audio Commands enumeration */
typedef enum
{
  AUDIO_CMD_START = 1,
  AUDIO_CMD_PLAY,
  AUDIO_CMD_STOP,
} AUDIO_CMD_TypeDef;


typedef enum
{
  AUDIO_OFFSET_NONE = 0,
  AUDIO_OFFSET_HALF,
  AUDIO_OFFSET_FULL,
  AUDIO_OFFSET_UNKNOWN,
} AUDIO_OffsetTypeDef;





#if 0
typedef struct
{
  uint8_t cmd;
  uint8_t data[USB_EP0_MAXSIZE];
  uint8_t len;
  uint8_t unit;
} usbdAudioCtrl_t;
#endif


typedef struct {
  volatile uint32_t             alt_setting;
  volatile uint8_t              altCtrl;
  volatile uint8_t              altOut;
  volatile uint8_t              altIn;

  uint8_t                       *outBufPtr;
  uint16_t                      outBufSize;     /* bytes_per_packet * cnt + epsize */
  uint16_t                      outBufPosWr;
  uint16_t                      outBufPosRd;

  /* interface number */
  uint8_t                       ifCtrl;
  uint8_t                       ifOut;
  uint8_t                       ifIn;
  /* endpoint number */
  uint8_t                       epOut;
  uint8_t                       epIn;
  uint8_t                       epFb;
  /* size information */
  uint16_t                      epSizeOut;
  uint16_t                      epSizeIn;
  uint16_t                      pktDefaultSizeOut;
  uint16_t                      pktDefaultSizeIn;
} usbdAudioHandle_t;


typedef struct {
  int        (*init)         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
  int        (*deinit)       (uint32_t options);
  int        (*audioCmd)     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
  int        (*volumeCtl)    (uint8_t vol);
  int        (*muteCtl)      (uint8_t cmd);
  int        (*periodicTC)   (uint8_t cmd);
  int        (*getState)     (void);
} usbdAudioCb_t;


/*extern USBD_ClassTypeDef        USBD_AUDIO;*/
/*#define USBD_AUDIO_CLASS        &USBD_AUDIO*/


uint8_t  USBD_AUDIO_RegisterInterface(int drc, usbdAudioCb_t *fops);
void     UsbdAudioSync(int drc, AUDIO_OffsetTypeDef offset);

usbdifStatus_t  UsbdAudioGetRecvPointer(int drc, uint8_t **ptr, int *psz, int *pszRecv);
usbdifStatus_t  UsbdAudioAddRecvPointer(int drc, int sz);



#ifdef  _USBDAUDIO_C_
/* callback entry point from usbdif.c */
static usbdifStatus_t   UsbdAudioCbInit(usbdifClassDef_t *prc, int speed);
static usbdifStatus_t   UsbdAudioCbDeInit(usbdifClassDef_t *prc, int speed);
static usbdifStatus_t   UsbdAudioCbSetup(usbdifClassDef_t *prc, usbifSetup_t *req);
static usbdifStatus_t   UsbdAudioCbDataIn(usbdifClassDef_t *prc, uint8_t epnum);
static usbdifStatus_t   UsbdAudioCbDataOut(usbdifClassDef_t *prc, uint8_t epnum, int size);
static usbdifStatus_t   UsbdAudioCbSof(usbdifClassDef_t *prc, usbifSof_t *sof);

/*** usb audio function */
static usbdifStatus_t   UsbdAudioStandardGetInterface(usbdifClassDef_t *prc, usbifSetup_t *s);
static usbdifStatus_t   UsbdAudioStandardSetInterface(usbdifClassDef_t *prc, usbifSetup_t *s);



static uint8_t  *UsbdAudioGetCfgDesc(uint16_t *length);
static uint8_t  *UsbdAudioGetDeviceQualifierDesc(uint16_t *length);
#if 0
static uint8_t  UsbdAudioEP0_RxReady(usbdifClassDef_t *prc);
static uint8_t  UsbdAudioEP0_TxReady(usbdifClassDef_t *prc);
static uint8_t  UsbdAudioIsoINIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  UsbdAudioIsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static void     AUDIO_REQ_GetCurrent(usbdifClassDef_t *prc, USBD_SetupReqTypedef *req);
static void     AUDIO_REQ_SetCurrent(usbdifClassDef_t *prc, USBD_SetupReqTypedef *req);
#endif
#endif


#endif

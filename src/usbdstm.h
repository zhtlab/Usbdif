#ifndef _USBDSTM_H_
#define _USBDSTM_H_



#ifdef _USBDSTM_C_
static void     UsbdevTask(const void *arg);

static uint8_t  UsbdstmCompositeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  UsbdstmCompositeDeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  UsbdstmCompositeSetup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *setup);
static uint8_t  UsbdstmCompositeDataInDone(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  UsbdstmCompositeDataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  UsbdstmCompositeEp0RxReady(USBD_HandleTypeDef *pdev);
static uint8_t  UsbdstmCompositeEp0TxReady(USBD_HandleTypeDef *pdev);
static uint8_t  UsbdstmCompositeSof(USBD_HandleTypeDef *pdev);
static uint8_t  UsbdstmCompositeIsoInIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  UsbdstmCompositeIsoOutIncomplete(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  *UsbdstmCompositeGetCfgDescHS(uint16_t *length);
static uint8_t  *UsbdstmCompositeGetCfgDescFS(uint16_t *length);
static uint8_t  *UsbdstmCompositeGetDeviceQualifierDesc(uint16_t *length);
static uint8_t  *UsbdstmCompositeGetUserStringDesc(USBD_HandleTypeDef *pdev, uint8_t index, uint16_t *length);
#endif


#endif

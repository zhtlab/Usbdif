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

#ifndef _USB_DEF_H_
#define _USB_DEF_H_

/*** usb setup packet bmRequest */
#define USB_BMREQ_DIR_SHIFT             7
#define USB_BMREQ_DIR_MASK              (1 << (USB_BMREQ_DIR_SHIFT))
#define USB_BMREQ_DIR_OUT               (0 << (USB_BMREQ_DIR_SHIFT))
#define USB_BMREQ_DIR_IN                (1 << (USB_BMREQ_DIR_SHIFT))
#define USB_BMREQ_TYPE_SHIFT            5
#define USB_BMREQ_TYPE_MASK             (3 << (USB_BMREQ_TYPE_SHIFT))
#define USB_BMREQ_TYPE_STANDARD         (0 << (USB_BMREQ_TYPE_SHIFT))
#define USB_BMREQ_TYPE_CLASS            (1 << (USB_BMREQ_TYPE_SHIFT))
#define USB_BMREQ_TYPE_VENDOR           (2 << (USB_BMREQ_TYPE_SHIFT))
#define USB_BMREQ_TYPE_RESERVED         (3 << (USB_BMREQ_TYPE_SHIFT))
#define USB_BMREQ_RECIPIENT_SHIFT       0
#define USB_BMREQ_RECIPIENT_MASK        (0x1f << (USB_BMREQ_RECIPIENT_SHIFT))
#define USB_BMREQ_RECIPIENT_DEVICE      (0 << (USB_BMREQ_RECIPIENT_SHIFT))
#define USB_BMREQ_RECIPIENT_INTERFACE   (1 << (USB_BMREQ_RECIPIENT_SHIFT))
#define USB_BMREQ_RECIPIENT_ENDPOINT    (2 << (USB_BMREQ_RECIPIENT_SHIFT))
#define USB_BMREQ_RECIPIENT_OTHER       (3 << (USB_BMREQ_RECIPIENT_SHIFT))

/*** usb setup packet bRequest */
#define USB_BREQ_GET_STATUS                             0x00
#define USB_BREQ_CLEAR_FEATURE                          0x01
#define USB_BREQ_SET_FEATURE                            0x03
#define USB_BREQ_SET_ADDRESS                            0x05
#define USB_BREQ_GET_DESCRIPTOR                         0x06
#define USB_BREQ_SET_DESCRIPTOR                         0x07
#define USB_BREQ_GET_CONFIGURATION                      0x08
#define USB_BREQ_SET_CONFIGURATION                      0x09
#define USB_BREQ_GET_INTERFACE                          0x0A
#define USB_BREQ_SET_INTERFACE                          0x0B
#define USB_BREQ_SYNCH_FRAME                            0x0C


/*** descritpor types  MSB of wValue */
#define  USB_DESC_TYPE_DEVICE                           1
#define  USB_DESC_TYPE_CONFIGURATION                    2
#define  USB_DESC_TYPE_STRING                           3
#define  USB_DESC_TYPE_INTERFACE                        4
#define  USB_DESC_TYPE_ENDPOINT                         5
#define  USB_DESC_TYPE_DEVICE_QUALIFIER                 6
#define  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        7
#define  USB_DESC_TYPE_INTERAFCE_POWER                  8
#define  USB_DESC_TYPE_OTG                              9
#define  USB_DESC_TYPE_DEBUG                            10
#define  USB_DESC_TYPE_INTERFACE_ASSOCIATION            11
#define  USB_DESC_TYPE_BOS                              0x0F
#define  USB_DESC_TYPE_DEVICE_CAPABILITY                0x10
#define  USB_DESC_TYPE_CS_UNDEFINED                     0x20
#define  USB_DESC_TYPE_CS_DEVICE                        0x21
#define  USB_DESC_TYPE_CS_CONFIGURATION                 0x22
#define  USB_DESC_TYPE_CS_STRING                        0x23
#define  USB_DESC_TYPE_CS_INTERFACE                     0x24
#define  USB_DESC_TYPE_CS_ENDPOINT                      0x25
#define  USB_DESC_TYPE_SS_ENDPOINT_COMPANION            0x30
#define  USB_DESC_TYPE_SSPLUS_ISOC_ENDPOINT_COMPANION   0x31


/*** device capability type  for BOS */
#define USB_DEVCAP_TYPE_WIRELESS                0x01
#define USB_DEVCAP_TYPE_USB20_EXTENSION         0x02
#define USB_DEVCAP_TYPE_SUPERSPEED_USB          0x03
#define USB_DEVCAP_TYPE_CONTAINER_ID            0x04
#define USB_DEVCAP_TYPE_PLATFORM                0x05
#define USB_DEVCAP_TYPE_POWER_DELIVERY_CAP      0x06
#define USB_DEVCAP_TYPE_BATTERY_INFO_CAP        0x07
#define USB_DEVCAP_TYPE_PD_CONSUMER_PORT_CAP    0x08
#define USB_DEVCAP_TYPE_PD_PROVIDER_PORT_CAP    0x09
#define USB_DEVCAP_TYPE_SUPERSPEED_PLUS         0x0a
#define USB_DEVCAP_TYPE_PRECISION_TIME_MEASUREMENT 0x0b
#define USB_DEVCAP_TYPE_WIRELESS_USB_EXT        0x0c


/*** the definition of  CLASSes and SUBCLASSes */
#define USB_CLASS_TYPE_RESERVED0        0
#define USB_CLASS_TYPE_AUDIO            1
#define         USB_SUBCLASS_TYPE_AUDIO_UNDEFINED       0
#define         USB_SUBCLASS_TYPE_AUDIO_CONTROL         1
#define         USB_SUBCLASS_TYPE_AUDIO_STREAMING       2
#define         USB_SUBCLASS_TYPE_AUDIO_MIDI            3

#define USB_CLASS_TYPE_CDC              2
#define         USB_SUBCLASS_TYPE_CDC_DLCM      1
#define         USB_SUBCLASS_TYPE_CDC_ACM       2
#define         USB_SUBCLASS_TYPE_CDC_TCM       3
#define         USB_SUBCLASS_TYPE_CDC_MCCM      4
#define         USB_SUBCLASS_TYPE_CDC_CCM       5
#define         USB_SUBCLASS_TYPE_CDC_ETH       6
#define         USB_SUBCLASS_TYPE_CDC_ATM       7
#define         USB_PROTCOL_ID_CDC_NONE         0
#define         USB_PROTCOL_ID_CDC_V25TER       1
#define         USB_PROTCOL_ID_CDC_VENDOR       0xff
#define         USB_CS_CDC_HEADER               0
#define         USB_CS_CDC_CALL_MANGEMENT       1
#define         USB_CS_CDC_ABSTRACT_CONTROL     2
#define         USB_CS_CDC_DIRECT_LINE_MANAGEMENT  3
#define         USB_CS_CDC_TELEPHONE_RINGER     4
#define         USB_CS_CDC_TELE_CALL_AND_LINE_STATE  5
#define         USB_CS_CDC_UNION                6
#define         USB_CS_CDC_COUNTRY_SELECTION    7
#define         USB_CS_CDC_TELEPHONE_OP_MODES   8
#define         USB_CS_CDC_USB_TERMINAL         9
#define         USB_CS_CDC_NETWORK_CHANNEL      0x0a
#define         USB_CS_CDC_PROTOCOL_UNIT        0x0b
#define         USB_CS_CDC_EXTENSION_UNIT       0x0c
#define         USB_CS_CDC_MULTI_CHANNEL_MANAGEMENT  0x0d
#define         USB_CS_CDC_CAPI_CONTROL_MANAGEMENT  0x0e
#define         USB_CS_CDC_ETHERNET_NETWORKING  0x0f
#define         USB_CS_CDC_ATM_NETWORKING       0x10
#define USB_CLASS_TYPE_HID              3
#define USB_CLASS_TYPE_RESERVED4        4
#define USB_CLASS_TYPE_PHYSICAL         5
#define USB_CLASS_TYPE_IMAGE            6
#define USB_CLASS_TYPE_PRINTER          7
#define USB_CLASS_TYPE_MASS_STORAGE     8
#define         USB_SUBCLASS_TYPE_MSC_NO_REPORTED  0
#define         USB_SUBCLASS_TYPE_MSC_RBC       1
#define         USB_SUBCLASS_TYPE_MSC_MMC5      2       /* atapi */
#define         USB_SUBCLASS_TYPE_MSC_QIC157    3       /* obsolete */
#define         USB_SUBCLASS_TYPE_MSC_UFI       4
#define         USB_SUBCLASS_TYPE_MSC_SFF8070I  5       /* obsolete */
#define         USB_SUBCLASS_TYPE_MSC_SCSI      6
#define         USB_SUBCLASS_TYPE_MSC_LSDFS     7
#define         USB_SUBCLASS_TYPE_MSC_IEEE1667  8
#define         USB_SUBCLASS_TYPE_MSC_VENDOR    0xff
#define USB_CLASS_TYPE_HUB              9
#define USB_CLASS_TYPE_CDC_DATA         10
#define USB_CLASS_TYPE_SMART_CARD       11
#define USB_CLASS_TYPE_SECURITY         12
#define USB_CLASS_TYPE_VIDEO            13
#define USB_CLASS_TYPE_END_MAJOR        14
#define USB_CLASS_TYPE_DIAG             0xdc
#define USB_CLASS_TYPE_WIRESS           0xe0
#define USB_CLASS_TYPE_APP_SPECIFIC     0xfe
#define USB_CLASS_TYPE_VENDOR           0xff


/*** string indexes */
#define USB_STRING_INDEX_LANGID                         0
#define USB_STRING_INDEX_MFC                            1
#define USB_STRING_INDEX_PRODUCT                        2
#define USB_STRING_INDEX_SERIAL                         3
#define USB_STRING_INDEX_CONFIG                         4
#define USB_STRING_INDEX_INTERFACE                      5


/*** endpoint type definitons */
#define USB_EP_TYPE_TRANSFAR_SHIFT 0
#define USB_EP_TYPE_TRANSFAR_MASK  (3 << (USB_EP_TYPE_TRANSFAR_SHIFT))
#define USB_EP_TYPE_CTRL        (0 << (USB_EP_TYPE_TRANSFAR_SHIFT))
#define USB_EP_TYPE_ISOC        (1 << (USB_EP_TYPE_TRANSFAR_SHIFT))
#define USB_EP_TYPE_BULK        (2 << (USB_EP_TYPE_TRANSFAR_SHIFT))
#define USB_EP_TYPE_INTR        (3 << (USB_EP_TYPE_TRANSFAR_SHIFT))

#define USB_EP_TYPE_SYNC_SHIFT  2
#define USB_EP_TYPE_SYNC_MASK   (3 << (USB_EP_TYPE_SYNC_SHIFT))
#define USB_EP_TYPE_NOSYNC      (0 << (USB_EP_TYPE_SYNC_SHIFT))
#define USB_EP_TYPE_ASYNC       (1 << (USB_EP_TYPE_SYNC_SHIFT))
#define USB_EP_TYPE_ADAPTIVE    (2 << (USB_EP_TYPE_SYNC_SHIFT))
#define USB_EP_TYPE_SYNC        (3 << (USB_EP_TYPE_SYNC_SHIFT))

#define USB_EP_TYPE_USAGE_SHIFT 4
#define USB_EP_TYPE_USAGE_MASK  (3 << (USB_EP_TYPE_USAGE_SHIFT))
#define USB_EP_TYPE_DATA        (0 << (USB_EP_TYPE_USAGE_SHIFT))
#define USB_EP_TYPE_FEEDBACK    (1 << (USB_EP_TYPE_USAGE_SHIFT))
#define USB_EP_TYPE_IMPLICIT    (2 << (USB_EP_TYPE_USAGE_SHIFT))
#define USB_EP_TYPE_RESERVED    (3 << (USB_EP_TYPE_USAGE_SHIFT))


#define USB_STR_DESC_MAXSIZE   255



#define USB_EP0_MAXSIZE            64


#endif

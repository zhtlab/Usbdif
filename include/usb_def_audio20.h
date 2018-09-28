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

#ifndef _USB_DEF_AUDIO20_H_
#define _USB_DEF_AUDIO20_H_


/*******************************************************************
 * USB AUDIO 2.0 definitions
 */

/* A.3 Audio Interface Class Codes (Table A-3) */
#define USBAUDIO20_AF_PROTOCOL_UNDEF            0x00
#define USBAUDIO20_AF_VERSION_02_00             0x20

/* A.4 Audio Interface Class Codes (Table A-4) */
#define USBAUDIO20_AUDIO                        0x01

/* A.5 Audio Interface Subclass Code (Table A-5) */
#define USBAUDIO20_SUBCLASS_UNDEFINED           0x00
#define USBAUDIO20_AUDIOCONTROL                 0x01
#define USBAUDIO20_AUDIOSTREAMING               0x02
#define USBAUDIO20_MINISTREAMING                0x03

/* A.6 Audio Interface Protocol Codes (Table A-6) */
#define USBAUDIO20_IP_PROTOCOL_UNDEFINED        0x00
#define USBAUDIO20_IP_VERSION_02_00             0x20

/* A.7 Audio Function Category Codes  (Table A-7) */
#define USBAUDIO20_FUNCTION_SUBCLASS_UNDEFINED  0x00
#define USBAUDIO20_DESKTOP_SPEAKER              0x01
#define USBAUDIO20_HOME_THEATER                 0x02
#define USBAUDIO20_MICROPHONE                   0x03
#define USBAUDIO20_HEADSET                      0x04
#define USBAUDIO20_TELEPHONE                    0x05
#define USBAUDIO20_CONVERTER                    0x06
#define USBAUDIO20_VOICE_SOUND_RECORDER         0x07
#define USBAUDIO20_IO_BOX                       0x08
#define USBAUDIO20_MUSICAL_INSTRUMENT           0x09
#define USBAUDIO20_PRO_AUDIO                    0x0a
#define USBAUDIO20_AUDIO_VIDEO                  0x0b
#define USBAUDIO20_CONTROL_PANEL                0x0c
#define USBAUDIO20_OTHER                        0xff

/* A.8 Audio Class-Specific Descriptor Codes  (Table A-8) */
#define USBAUDIO20_CS_UNDEFINED                 0x20
#define USBAUDIO20_CS_DEVICE                    0x21
#define USBAUDIO20_CS_CONFIGURATION             0x22
#define USBAUDIO20_CS_STRING                    0x23
#define USBAUDIO20_CS_INTERFACE                 0x24
#define USBAUDIO20_CS_ENDPOINT                  0x25

/* A.9 Audio Class-Specific AC Interface Descriptor Subtypes (Table A-9) */
#define USBAUDIO20_AC_DESCRIPTOR_UNDEFINED      0x00
#define USBAUDIO20_HEADER                       0x01
#define USBAUDIO20_INPUT_TERMINAL               0x02
#define USBAUDIO20_OUTPUT_TERMINAL              0x03
#define USBAUDIO20_MIXER_UNIT                   0x04
#define USBAUDIO20_SELECTOR_UNIT                0x05
#define USBAUDIO20_FEATURE_UNIT                 0x06
#define USBAUDIO20_EFFECT_UNIT                  0x07
#define USBAUDIO20_PROCESSING_UNIT              0x08
#define USBAUDIO20_EXTENSION_UNIT               0x09
#define USBAUDIO20_CLOCK_SOURCE                 0x0a
#define USBAUDIO20_CLOCK_SELECTOR               0x0b
#define USBAUDIO20_CLOCK_MULTIPLIER             0x0c
#define USBAUDIO20_SAMPLING_RATE_CONVERTER      0x0d

/* A.10 Audio Class-Specific AS Interface Descriptor Subtypes (Table A-10) */
#define USBAUDIO20_AS_DESCRIPTOR_UNDEFINED      0x00
#define USBAUDIO20_AS_GENERAL                   0x01
#define USBAUDIO20_FORMAT_TYPE                  0x02
#define USBAUDIO20_ENCODER                      0x03
#define USBAUDIO20_DECODER                      0x04

/* A.11 Processing Unit Process Types (Table A-11) */
#define USBAUDIO20_EFFECT_UNDEFINED             0x00
#define USBAUDIO20_PARAM_EQ_SECTION_EFFECT      0x01
#define USBAUDIO20_REVERBERATION_EFFECT         0x02
#define USBAUDIO20_MOD_DELAY_EFFECT             0x03
#define USBAUDIO20_DYN_RAGE_COMP_EFFECT         0x04

/* A.12 Processing Unit Process Types (Table A-12) */
#define USBAUDIO20_PROCESS_UNDEFINED            0x00
#define USBAUDIO20_UPDOWNMIX_PROCESS            0x01
#define USBAUDIO20_DOLBY_PROLOGIC_PROCESS       0x02
#define USBAUDIO20_STEREO_EXTENDER_PROCESS      0x03

/* A.13 Audio Class-Specific Endpoint Descriptor Subtypes (Table A-13) */
#define USBAUDIO20_DESDCRIPTOR_UNDEFINED        0x00
#define USBAUDIO20_EP_GENERAL                   0x01

/* A.14 Audio Class-Specific Request Codes (Table A-14) */
#define USBAUDIO20_REQ_CODE_UNDEFINED           0x00
#define USBAUDIO20_CUR                          0x01
#define USBAUDIO20_RANGE                        0x02
#define USBAUDIO20_MEM                          0x03

/* A.15 Encoder Type Code (Table A-15) */
#define USBAUDIO20_ENCODER_UNDEFINED            0x00
#define USBAUDIO20_OTHER_ENCODER                0x01
#define USBAUDIO20_MPEG_ENCODER                 0x02
#define USBAUDIO20_AC3_ENCODER                  0x03
#define USBAUDIO20_WMA_ENCODER                  0x04
#define USBAUDIO20_DTS_ENCODER                  0x05

/* A.16 Decoder Type Code (Table A-16) */
#define USBAUDIO20_DECODER_UNDEFINED            0x00
#define USBAUDIO20_OTHER_DECODER                0x01
#define USBAUDIO20_MPEG_DECODER                 0x02
#define USBAUDIO20_AC3_DECODER                  0x03
#define USBAUDIO20_WMA_DECODER                  0x04
#define USBAUDIO20_DTS_DECODER                  0x05

/* A.17 Control Selector Codes */
/* A.17.1 Clock Source Control Selectors (Table A-17) */
#define USBAUDIO20_CS_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_CS_SAM_FREQ_CONTROL          0x01
#define USBAUDIO20_CS_CLOCK_VALID_CONTROL       0x02

/* A.17.2 Clock Source Control Selectors (Table A-18) */
#define USBAUDIO20_CX_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_CX_CLOCK_SELECTOR_CONTROL    0x01

/* A.17.3 Clock Source Control Selectors (Table A-19) */
#define USBAUDIO20_CN_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_CN_NUMERATOR_CONTROL         0x01
#define USBAUDIO20_CN_DENOMINATOR_CONTROL       0x02

/* A.17.4 Terminal Control Selectors (Table A-20) */
#define USBAUDIO20_TE_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_TE_COPY_PROTECT_CONTROL      0x01
#define USBAUDIO20_TE_CONNECTOR_CONTROL         0x02
#define USBAUDIO20_TE_OVERLOAD_CONTROL          0x03
#define USBAUDIO20_TE_CLUSTER_CONTROL           0x04
#define USBAUDIO20_TE_UNDERFLOW_CONTROL         0x05
#define USBAUDIO20_TE_OVERFLOW_CONTROL          0x06
#define USBAUDIO20_TE_LATENCY_CONTROL           0x07

/* A.17.5 Terminal Control Selectors (Table A-21) */
#define USBAUDIO20_MU_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_MU_MIXER_CONTROL             0x01
#define USBAUDIO20_MU_CLUSTER_CONTROL           0x02
#define USBAUDIO20_MU_UNDERFLOW_CONTROL         0x03
#define USBAUDIO20_MU_OVERFLOW_CONTROL          0x04
#define USBAUDIO20_MU_LATENCY_CONTROL           0x05

/* A.17.6 Terminal Control Selectors (Table A-22) */
#define USBAUDIO20_SU_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_SU_SELECTOR_CONTROL          0x01
#define USBAUDIO20_SU_LATENCY_CONTROL           0x02

/* A.17.7 Feature Unit Control Selectors (Table A-23) */
#define USBAUDIO20_FU_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_FU_MUTE_CONTROL              0x01
#define USBAUDIO20_FU_VOLUME_CONTROL            0x02
#define USBAUDIO20_FU_BASS_CONTROL              0x03
#define USBAUDIO20_FU_MID_CONTROL               0x04
#define USBAUDIO20_FU_TREBLE_CONTROL            0x05
#define USBAUDIO20_FU_GRAPHIC_EQUALIZER_CONTROL 0x06
#define USBAUDIO20_FU_AUTOMATIC_GAIN_CONTROL    0x07
#define USBAUDIO20_FU_DELAY__CONTROL            0x08
#define USBAUDIO20_FU_BASS_BOOST_CONTROL        0x09
#define USBAUDIO20_FU_LOUDNESS_CONTROL          0x0a
#define USBAUDIO20_FU_INPUT_GAIN_CONTROL        0x0b
#define USBAUDIO20_FU_INPUT_GAIN_PAD_CONTROL    0x0c
#define USBAUDIO20_FU_PHASE_INVERTER_CONTROL    0x0d
#define USBAUDIO20_FU_UNDERFLOW_CONTROL         0x0e
#define USBAUDIO20_FU_OVERFLOW_CONTROL          0x0f
#define USBAUDIO20_FU_LATENCY_CONTROL           0x10

/* A.17.8 Effect Unit Control Selectors */
/* A.17.8.1 Parametric Equalizer Section Effect Unit Control Selectors (Table A-24) */
#define USBAUDIO20_PE_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_PE_ENABLE_CONTROL            0x01
#define USBAUDIO20_PE_CENTERFREQ_CONTROL        0x02
#define USBAUDIO20_PE_QFACTOR_CONTROL           0x03
#define USBAUDIO20_PE_GAIN_CONTROL              0x04
#define USBAUDIO20_PE_UNDERFLOW_CONTROL         0x05
#define USBAUDIO20_PE_OVERFLOW_CONTROL          0x06
#define USBAUDIO20_PE_LATENCY_CONTROL           0x07

/* A.17.8.2 Reverberation Effect Unit Control Selectors (Table A-25) */
#define USBAUDIO20_RV_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_RV_ENABLE_CONTROL            0x01
#define USBAUDIO20_RV_TYPE_CONTROL              0x02
#define USBAUDIO20_RV_LEVEL_CONTROL             0x03
#define USBAUDIO20_RV_TIME_CONTROL              0x04
#define USBAUDIO20_RV_REEDBACK_CONTROL          0x05
#define USBAUDIO20_RV_PREDELAY_CONTROL          0x06
#define USBAUDIO20_RV_DENSITY_CONTROL           0x07
#define USBAUDIO20_RV_HIFREQ_ROLLOFF_CONTROL    0x08
#define USBAUDIO20_RV_UNDERFLOW_CONTROL         0x09
#define USBAUDIO20_RV_OVERFLOW_CONTROL          0x0a
#define USBAUDIO20_RV_LATENCY_CONTROL           0x0b

/* A.17.8.3 Reverberation Effect Unit Control Selectors (Table A-25) */
#define USBAUDIO20_MD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_MD_ENABLE_CONTROL            0x01
#define USBAUDIO20_MD_BALANCE_CONTROL           0x02
#define USBAUDIO20_MD_RATE_CONTROL              0x03
#define USBAUDIO20_MD_DEPTH_CONTROL             0x04
#define USBAUDIO20_MD_TIME_CONTROL              0x05
#define USBAUDIO20_MD_FEEDBACK_CONTROL          0x06
#define USBAUDIO20_MD_UNDERFLOW_CONTROL         0x07
#define USBAUDIO20_MD_OVERFLOW_CONTROL          0x08
#define USBAUDIO20_MD_LATENCY_CONTROL           0x09

/* A.17.8.4 Dynamic Range Compressor Processing Unit Control Selectors (Table A-27) */
#define USBAUDIO20_DR_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_DR_ENABLE_CONTROL            0x01
#define USBAUDIO20_DR_COMPRESSION_RATE_CONTROL  0x02
#define USBAUDIO20_DR_MAXAMPL_CONTROL           0x03
#define USBAUDIO20_DR_THRESHOLD_CONTROL         0x04
#define USBAUDIO20_DR_ATTACK_TIME               0x05
#define USBAUDIO20_DR_RELEASE_TIME              0x06
#define USBAUDIO20_DR_UNDERFLOW_CONTROL         0x07
#define USBAUDIO20_DR_OVERFLOW_CONTROL          0x08
#define USBAUDIO20_DR_LATENCY_CONTROL           0x09

/* A.17.9 Processing Unit Control Selectors */
/* A.17.9.1 Up/Down-mix Processing Unit Control Selectors (Table A-28) */
#define USBAUDIO20_UD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_UD_ENABLE_CONTROL            0x01
#define USBAUDIO20_UD_MODE_SELECT_CONTROL       0x02
#define USBAUDIO20_UD_CLUSTER_CONTROL           0x03
#define USBAUDIO20_UD_UNDERFLOW_CONTROL         0x04
#define USBAUDIO20_UD_OVERFLOW_CONTROL          0x05
#define USBAUDIO20_UD_LATENCY_CONTROL           0x06

/* A.17.9.2 Dolby Prologic(TM) Processing Unit Control Selectors (Table A-29) */
#define USBAUDIO20_DP_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_DP_ENABLE_CONTROL            0x01
#define USBAUDIO20_DP_MODE_SELECT_CONTROL       0x02
#define USBAUDIO20_DP_CLUSTER_CONTROL           0x03
#define USBAUDIO20_DP_UNDERFLOW_CONTROL         0x04
#define USBAUDIO20_DP_OVERFLOW_CONTROL          0x05
#define USBAUDIO20_DP_LATENCY_CONTROL           0x06

/* A.17.9.3 Stereo Extender Processing Unit Selectors (Table A-30) */
#define USBAUDIO20_ST_EXT_CONTROL_UNDEFINED     0x00
#define USBAUDIO20_ST_EXT_ENABLE_CONTROL        0x01
#define USBAUDIO20_ST_EXT_WIDTH_CONTROL         0x02
#define USBAUDIO20_ST_EXT_UNDERFLOW_CONTROL     0x03
#define USBAUDIO20_ST_EXT_OVERFLOW_CONTROL      0x04
#define USBAUDIO20_ST_EXT_LATENCY_CONTROL       0x05

/* A.17.10 Extension Unit Control Selectors (Table A-31) */
#define USBAUDIO20_XU_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_XU_ENABLE_CONTROL            0x01
#define USBAUDIO20_XU_CLUSTER_CONTROL           0x02
#define USBAUDIO20_XU_UNDERFLOW_CONTROL         0x03
#define USBAUDIO20_XU_OVERFLOW_CONTROL          0x04
#define USBAUDIO20_XU_LATENCY_CONTROL           0x05

/* A.17.11 Extension Unit Control Selectors (Table A-32) */
#define USBAUDIO20_AS_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_AS_ACT_ALT_SETTING_CONTROL   0x01
#define USBAUDIO20_AS_VAL_ALT_SETTINGS_CONTROL  0x02
#define USBAUDIO20_AS_AUDIO_DATA_FORMAT_CONTROL 0x03

/* A.17.12 Extension Unit Control Selectors (Table A-33) */
#define USBAUDIO20_EN_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_EN_BIT_RATE_CONTROL          0x01
#define USBAUDIO20_EN_QUALITY_CONTROL           0x02
#define USBAUDIO20_EN_VBR_CONTROL               0x03
#define USBAUDIO20_EN_TYPE_CONTROL              0x04
#define USBAUDIO20_EN_UNDERFLOW_CONTROL         0x05
#define USBAUDIO20_EN_OVERFLOW_CONTROL          0x06
#define USBAUDIO20_EN_ENCODER_ERROR_CONTROL     0x07
#define USBAUDIO20_EN_PARAM1_CONTROL            0x08
#define USBAUDIO20_EN_PARAM2_CONTROL            0x09
#define USBAUDIO20_EN_PARAM3_CONTROL            0x0a
#define USBAUDIO20_EN_PARAM4_CONTROL            0x0b
#define USBAUDIO20_EN_PARAM5_CONTROL            0x0c
#define USBAUDIO20_EN_PARAM6_CONTROL            0x0d
#define USBAUDIO20_EN_PARAM7_CONTROL            0x0e
#define USBAUDIO20_EN_PARAM8_CONTROL            0x0f

/* A.17.13 Decoder Control Selectors */
/* A.17.13.1 MPEG Decoder Control Selectors (Table A-34) */
#define USBAUDIO20_MD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_MD_DUAL_CHANNEL_CONTROL      0x01
#define USBAUDIO20_MD_SECOND_STEREO_CONTROL     0x02
#define USBAUDIO20_MD_MULTILINGUAL_CONTROL      0x03
#define USBAUDIO20_MD_DYN_RANGE_CONTROL         0x04
#define USBAUDIO20_MD_SCALING_CONTROL           0x05
#define USBAUDIO20_MD_HILO_SCALING_CONTROL      0x06
#define USBAUDIO20_MD_UNDERFLOW_CONTROL         0x07
#define USBAUDIO20_MD_OVERFLOW_CONTROL          0x08
#define USBAUDIO20_MD_DECODER_ERROR_CONTROL     0x09

/* A.17.13.2 AC3 Decoder Control Selectors (Table A-35) */
#define USBAUDIO20_AD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_AD_DUAL_CHANNEL_CONTROL      0x01
#define USBAUDIO20_AD_DYN_RANGE_CONTROL         0x02
#define USBAUDIO20_AD_SCALING_CONTROL           0x03
#define USBAUDIO20_AD_HILO_SCALING_CONTROL      0x04
#define USBAUDIO20_AD_UNDERFLOW_CONTROL         0x05
#define USBAUDIO20_AD_OVERFLOW_CONTROL          0x06
#define USBAUDIO20_AD_DECODER_ERROR_CONTROL     0x07

/* A.17.13.3 WMA Decoder Control Selectors (Table A-36) */
#define USBAUDIO20_WD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_WD_UNDERFLOW_CONTROL         0x01
#define USBAUDIO20_WD_OVERFLOW_CONTROL          0x02
#define USBAUDIO20_WD_DECODER_ERROR_CONTROL     0x03

/* A.17.13.4 WMA Decoder Control Selectors (Table A-37) */
#define USBAUDIO20_DD_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_DD_UNDERFLOW_CONTROL         0x01
#define USBAUDIO20_DD_OVERFLOW_CONTROL          0x02
#define USBAUDIO20_DD_DECODER_ERROR_CONTROL     0x03

/* A.17.14 Endpoint Control Selectors (Table A-38) */
#define USBAUDIO20_EP_CONTROL_UNDEFINED         0x00
#define USBAUDIO20_EP_PTICH_CONTROL             0x01
#define USBAUDIO20_EP_DATA_UNDERFLOW_CONTROL    0x02
#define USBAUDIO20_EP_DATA_OVERFLOW_CONTROL     0x03


/*******************************************************************
 * USB AUDIO FORMAT 2.0 definitions
 */
/* A.3 Audio Interface Class Codes (Table A-3) */
#define USBAUDIOFORMAT20_TYPE_UNDEF             0x00
#define USBAUDIOFORMAT20_TYPE_I                 0x01
#define USBAUDIOFORMAT20_TYPE_II                0x02
#define USBAUDIOFORMAT20_TYPE_III               0x03
#define USBAUDIOFORMAT20_TYPE_IV                0x04
#define USBAUDIOFORMAT20_EXT_TYPE_I             0x81
#define USBAUDIOFORMAT20_EXT_TYPE_II            0x82
#define USBAUDIOFORMAT20_EXT_TYPE_III           0x83


#endif  /* _USB_DEF_AUDIO20_H_ */

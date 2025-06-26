/**
  ******************************************************************************
  * @file    BSP/Inc/main.h 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    30-December-2016 
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_sd.h"
#include "stm32746g_discovery_eeprom.h"
#include "stm32746g_discovery_camera.h"
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery_qspi.h"

#include "usbh_MIDI.h"
#include "usbh_def.h"

/* Macros --------------------------------------------------------------------*/
#ifdef USE_FULL_ASSERT
/* Assert activated */
#define ASSERT(__condition__)                do { if(__condition__) \
                                                   {  assert_failed(__FILE__, __LINE__); \
                                                      while(1);  \
                                                    } \
                                              }while(0)
#else
/* Assert not activated : macro has no effect */
/*#define ASSERT(__condition__)                  do { if(__condition__) \
                                                   {  ErrorCounter++; \
                                                    } \
                                              }while(0)*/
#endif

#define RGB565_BYTE_PER_PIXEL     2
#define ARBG8888_BYTE_PER_PIXEL   4

/* Camera have a max resolution of VGA : 640x480 */
#define CAMERA_RES_MAX_X          640
#define CAMERA_RES_MAX_Y          480

/**
  * @brief  LCD FB_StartAddress
  * LCD Frame buffer start address : starts at beginning of SDRAM
  */
#define LCD_FRAME_BUFFER          SDRAM_DEVICE_ADDR

/**
  * @brief  Camera frame buffer start address
  * Assuming LCD frame buffer is of size 480x800 and format ARGB8888 (32 bits per pixel).
  */
#define CAMERA_FRAME_BUFFER       ((uint32_t)(LCD_FRAME_BUFFER + (RK043FN48H_WIDTH * RK043FN48H_HEIGHT * ARBG8888_BYTE_PER_PIXEL)))

/**
  * @brief  SDRAM Write read buffer start address after CAM Frame buffer
  * Assuming Camera frame buffer is of size 640x480 and format RGB565 (16 bits per pixel).
  */
#define SDRAM_WRITE_READ_ADDR        ((uint32_t)(CAMERA_FRAME_BUFFER + (CAMERA_RES_MAX_X * CAMERA_RES_MAX_Y * RGB565_BYTE_PER_PIXEL)))

#define SDRAM_WRITE_READ_ADDR_OFFSET ((uint32_t)0x0800)
#define SRAM_WRITE_READ_ADDR_OFFSET  SDRAM_WRITE_READ_ADDR_OFFSET

#define AUDIO_REC_START_ADDR         SDRAM_WRITE_READ_ADDR

// added 16 May DSR
#define PING 0
#define PONG 1


#define OTG_FS_ID_Pin GPIO_PIN_10
#define OTG_FS_ID_GPIO_Port GPIOA
#define OTG_FS_OverCurrent_Pin GPIO_PIN_4
#define OTG_FS_OverCurrent_GPIO_Port GPIOD
#define OTG_FS_VBUS_Pin GPIO_PIN_12
#define OTG_FS_VBUS_GPIO_Port GPIOJ
#define OTG_FS_P_Pin GPIO_PIN_12
#define OTG_FS_P_GPIO_Port GPIOA
#define OTG_HS_OverCurrent_Pin GPIO_PIN_3
#define OTG_HS_OverCurrent_GPIO_Port GPIOE
#define OTG_FS_PowerSwitchOn_Pin GPIO_PIN_5
#define OTG_FS_PowerSwitchOn_GPIO_Port GPIOD
#define OTG_FS_N_Pin GPIO_PIN_11
#define OTG_FS_N_GPIO_Port GPIOA




#define MIDI_CC_BT_PLAY 41
#define MIDI_CC_BT_STOP 42
#define MIDI_CC_BT_REWIND 43
#define MIDI_CC_BT_LEFT 61
#define MIDI_CC_BT_RIGHT 62
#define MIDI_CC_BT_TRACK_LEFT 58
#define MIDI_CC_BT_TRACK_RIGHT 59

#define MIDI_CC_SLIDER1 0
#define MIDI_CC_SLIDER2 1

#define MIDI_CC_KNOB1 16
#define MIDI_CC_KNOB2 17
#define MIDI_CC_KNOB3 18
#define MIDI_CC_KNOB4 19
#define MIDI_CC_KNOB5 20

#define MIDI_CC_BT_S1 32
#define MIDI_CC_BT_S2 33
#define MIDI_CC_BT_S3 34
#define MIDI_CC_BT_S4 35
#define MIDI_CC_BT_S5 36
#define MIDI_CC_BT_S6 37
#define MIDI_CC_BT_S7 38
#define MIDI_CC_BT_S8 39

#define MIDI_CC_BT_M1 48
#define MIDI_CC_BT_M2 49
#define MIDI_CC_BT_M3 50
#define MIDI_CC_BT_M4 51
#define MIDI_CC_BT_M5 52
#define MIDI_CC_BT_M6 53
#define MIDI_CC_BT_M7 54
#define MIDI_CC_BT_M8 55

#define MIDI_BUF_SIZE 64

typedef enum {
	APP_IDLE = 0, APP_START, APP_READY, APP_RUNNING, APP_DISCONNECT
} AppState;


typedef enum {
	USBH_USER_FS_INIT = 0, USBH_USER_AUDIO
} USBAppState;



/* Exported types ------------------------------------------------------------*/

/*typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_NOTREADY,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
}AUDIO_ErrorTypeDef;
*/

//extern const unsigned char stlogo[];
/* Exported variables ---------------------------------------------------*/
//extern uint8_t     NbLoop;
//extern uint8_t     MfxExtiReceived;
//#ifndef USE_FULL_ASSERT
//extern uint32_t    ErrorCounter;
//#endif
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define COUNT_OF_EXAMPLE(x)    (sizeof(x)/sizeof(BSP_DemoTypedef))
/* Exported functions ------------------------------------------------------- */
void AudioLoopback_demo (void);
uint8_t CheckForUserInput(void);
void BSP_LCD_DMA2D_IRQHandler(void);
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line);
#endif
#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

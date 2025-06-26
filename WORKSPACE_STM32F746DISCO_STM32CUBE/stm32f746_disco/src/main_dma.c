#include "FIR_coeff.h"
#include "system_config.h"
#include "arm_math.h"
#include "FIR_filter.h"
#include "IIR_filter.h"
#include "stm32f7_wm8994_init.h"
#include "stm32f7_display.h"
#include "main.h"
#include "notes.h"
#include "arm_math.h"
#include "tickTimer.h"
#include "signalTables.h"

#define SOURCE_FILE_NAME "Titre Graphe"

static void usbUserProcess(USBH_HandleTypeDef *pHost, uint8_t vId);
static void midiApplication();
static __IO uint32_t USBReceiveAvailable = 0;
USBH_HandleTypeDef hUSBHost; /* USB Host handle */
static uint8_t midiReceiveBuffer[MIDI_BUF_SIZE];
static __IO uint32_t canReceive = 0;
static AppState appState = APP_IDLE;

extern int16_t rx_sample_L;
extern int16_t rx_sample_R;
extern int16_t tx_sample_L;
extern int16_t tx_sample_R;

float32_t h_average[5];

uint8_t noteOn=0;
uint8_t noteOff=0;

arm_fir_instance_f32 ARM_FIR_F32;
float32_t state_f32[N_FILTER];

extern volatile int32_t TX_buffer_empty; // these may not need to be int32_t
extern volatile int32_t RX_buffer_full; // they were extern volatile int16_t in F4 version
extern int16_t rx_buffer_proc, tx_buffer_proc; // will be assigned token values PING or PONG
void process_buffer(void);

float32_t x[N_FILTER], y[PING_PONG_BUFFER_SIZE];
float32_t xright[PING_PONG_BUFFER_SIZE], xleft[PING_PONG_BUFFER_SIZE], y[PING_PONG_BUFFER_SIZE], state[N_FILTER+PING_PONG_BUFFER_SIZE-1];
arm_fir_instance_f32 S;


int main(void)
{
	  HAL_Init();
	  MPU_Config();         // configure the MPU attributes as write-through
	  CPU_CACHE_Enable();   // enable the CPU cache
	  SystemClock_Config(); // configure the system clock to 200 Mhz

	  BSP_LED_Init(LED1);   // initialise LED on GPIO pin P   (also accessible on arduino header)
	  BSP_GPIO_Init();      // initialise diagnostic GPIO pin P   (accessible on arduino header)
	  BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO); // configure the  blue user pushbutton in GPIO mode
	  BSP_SDRAM_Init();

	  HAL_Delay(200);
	//#############################################
	// 			FILTRE  INIT
	//#############################################
	 arm_fir_init_f32(&ARM_FIR_F32, N_FILTER, FILTER_COEFFS, state, PING_PONG_BUFFER_SIZE); // DMA
	//#############################################

	USBH_Init(&hUSBHost, usbUserProcess, 0);
	USBH_RegisterClass(&hUSBHost, USBH_MIDI_CLASS);
	USBH_Start(&hUSBHost);

  stm32f7_wm8994_init(AUDIO_FREQUENCY_44K,
                      IO_METHOD_DMA,
                      INPUT_DEVICE_INPUT_LINE_1,
                      OUTPUT_DEVICE_HEADPHONE,
                      WM8994_HP_OUT_ANALOG_GAIN_0DB,
                      WM8994_LINE_IN_GAIN_0DB,
                      WM8994_DMIC_GAIN_9DB,
                      SOURCE_FILE_NAME,
                      NOGRAPH);

  while(1)
  {
		midiApplication();
		USBH_Process(&hUSBHost);

		// DMA
	    while(!(RX_buffer_full && TX_buffer_empty)){ // Il faut que rx_buff soit plein ET que tx_buff soit vide pour traiter les données
	    }
	    process_buffer();
  }
}

//###########################################
// 				DMA
//###########################################

void process_buffer(void) // this function processes one DMA transfer block worth of data
{
  int i,k;
  int16_t *rx_buf, *tx_buf;

  if (rx_buffer_proc == PING) {rx_buf = (int16_t *)PING_IN;}
  else {rx_buf = (int16_t *)PONG_IN;}
  if (tx_buffer_proc == PING) {tx_buf = (int16_t *)PING_OUT;}
  else {tx_buf = (int16_t *)PONG_OUT;}
  BSP_LED_On(LED1);

  for (i=0 ; i<(PING_PONG_BUFFER_SIZE) ; i++)
  {
	  xleft[i] = *rx_buf++;
	  xleft[i] = *rx_buf++;
  }

  BSP_LED_On(LED1);
  arm_fir_f32(&ARM_FIR_F32,xleft,y,PING_PONG_BUFFER_SIZE);
  BSP_LED_Off(LED1);

  for (i=0 ; i<(PING_PONG_BUFFER_SIZE) ; i++)
  {
    *tx_buf++ = (short)(y[i]);
    *tx_buf++ = (short)(y[i]);
  }
  RX_buffer_full = 0;
  TX_buffer_empty = 0;
}

void usbUserProcess(USBH_HandleTypeDef *usbHost, uint8_t eventID) {
	UNUSED(usbHost);
	switch (eventID) {
	case HOST_USER_SELECT_CONFIGURATION:
		break;
	case HOST_USER_DISCONNECTION:
		appState = APP_DISCONNECT;
		BSP_LED_Off(LED_GREEN);
		break;
	case HOST_USER_CLASS_ACTIVE:
		appState = APP_READY;
		BSP_LED_On(LED_GREEN);
		break;
	case HOST_USER_CONNECTION:
		appState = APP_START;
		break;
	default:
		break;
	}
}

void midiApplication(void)
{
	switch (appState) {
	case APP_READY:
		USBH_MIDI_Receive(&hUSBHost, midiReceiveBuffer, MIDI_BUF_SIZE);
		//initAudio();
		//pausePlayback();
		//initSequencer();
		appState = APP_RUNNING;
		break;
	case APP_RUNNING:
		if (canReceive) {
			canReceive = 0;
			USBH_MIDI_Receive(&hUSBHost, midiReceiveBuffer, MIDI_BUF_SIZE);
		}
		break;
	case APP_DISCONNECT:
		appState = APP_IDLE;
		USBH_MIDI_Stop(&hUSBHost);
		break;
	default:
		break;
	}
}

void processMidiPackets() {
	uint8_t *ptr = midiReceiveBuffer;
	uint16_t numPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost)/4; //>> 2;

	if (numPackets != 0) {
		while (numPackets--) {
						ptr++;
						uint32_t type = *ptr++;
						uint32_t subtype = *ptr++;
						uint32_t val = *ptr++;


						if ((type & 0xF0) == 0x90)
								{
									noteOn = subtype;
								}

						if ((type & 0xF0) == 0x80)
								{
									noteOff = subtype;
								}

							}
						}
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost) {

	processMidiPackets();
	USBReceiveAvailable = 1;
}




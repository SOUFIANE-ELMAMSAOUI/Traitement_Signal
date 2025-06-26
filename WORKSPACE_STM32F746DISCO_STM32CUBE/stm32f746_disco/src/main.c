#include "FIR_coeff.h"
#include "system_config.h"
#include "arm_math.h"
#include "FIR_filter.h"
#include "IIR_filter.h"
#include "stm32f7_wm8994_init.h"
#include "main.h"
#include "notes.h"
#include "arm_math.h"
#include "tickTimer.h"
#include "signalTables.h"
#include "reverb.h"
#include "adsr.h"

#pragma GCC optimize ("O0")

// ============== Variables globales ==============

USBH_HandleTypeDef hUSBHost;
static uint8_t midiReceiveBuffer[MIDI_BUF_SIZE];
static __IO uint32_t USBReceiveAvailable = 0;
static AppState appState = APP_IDLE;

extern int16_t rx_sample_L;
extern int16_t rx_sample_R;
extern int16_t tx_sample_L;
extern int16_t tx_sample_R;

// Variables synthétiseur
float Fwave = 0.0f;
uint8_t note_active = 0;
uint8_t current_note = 0;
float k = 1.0f;

uint8_t note_pending = 0;
uint8_t pending_active = 0;

// Variables filtre FIR
#define N_FILTER 64
arm_fir_instance_f32 fir;
float32_t firCoeffs[N_FILTER];
float32_t firState[N_FILTER + N_FILTER - 1];

struct adsr_TypeStruct adsr_envelope;
struct reverb_TypeStruct reverb_left;
struct reverb_TypeStruct reverb_right;

#define FILTER_COEFFS h_low_0_4500__f32
#define CARRE_TABLE_SIZE 20
#define TRIANGLE_TABLE_SIZE 20
#define SAWTOOTH_TABLE_SIZE 20

static void usbUserProcess(USBH_HandleTypeDef *pHost, uint8_t vId);
static void midiApplication(void);
void update_filter_cutoff(float note_freq);
void processMidiPackets(void);
void init_synthesizer(void);
void debug_adsr_visual(void);

void update_filter_cutoff(float note_freq) {
    float cutoff = k * note_freq;
    if (cutoff > 4000.0f) cutoff = 4000.0f;
    if (cutoff < 20.0f) cutoff = 20.0f;
    FIR_calc_coeff_f32(&fir, N_FILTER, 0, cutoff, 44100.0f, 0);
}

void debug_adsr_visual() {
    static uint32_t debug_counter = 0;

    debug_counter++;
    if (debug_counter % 22050 == 0) {
        if(pending_active) {
            if (debug_counter % 2205 == 0) BSP_LED_Toggle(LED1);
        }
        else {
            switch(adsr_envelope.state) {
                case INIT:
                    BSP_LED_Off(LED1);
                    break;
                case ATTACK:
                    BSP_LED_On(LED1);
                    break;
                case DECAY:
                    BSP_LED_Toggle(LED1);
                    break;
                case SUSTAIN:
                    if (debug_counter % 44100 == 0) BSP_LED_Toggle(LED1);
                    break;
                case RELEASE:
                    if (debug_counter % 11025 == 0) BSP_LED_Toggle(LED1);
                    break;
            }
        }
    }
}

void BSP_AUDIO_SAI_Interrupt_CallBack() {
    static float phase = 0.0f;
    static float step = 0.0f;
    float32_t input, filtered_output, adsr_output, reverb_output_L, reverb_output_R;
    float envelope_level;
    int16_t sample;

    if (Fwave > 0.0f) {
        step = Fwave * CARRE_TABLE_SIZE / 44100.0f;
        phase += step;
        if (phase >= CARRE_TABLE_SIZE) {
            phase -= CARRE_TABLE_SIZE;
        }
        sample = carre_int[(int)phase];
    } else {
        sample = 0;
        phase = 0.0f;
    }

    input = (float32_t)sample / 32768.0f;
    arm_fir_f32(&fir, &input, &filtered_output, 1);

    envelope_level = adsr(&adsr_envelope);
    adsr_output = filtered_output * envelope_level;

    reverb_output_L = reverb_process(&reverb_left, adsr_output);
    reverb_output_R = reverb_process(&reverb_right, adsr_output);

    debug_adsr_visual();

    tx_sample_L = (int16_t)(reverb_output_L * 16384.0f);
    tx_sample_R = (int16_t)(reverb_output_R * 16384.0f);
}

void BSP_AUDIO_SAI_Interrupt_CallBack_TEST_ENVELOPE() {
    static uint32_t test_counter = 0;
    float envelope_level;
    float final_output;

    envelope_level = adsr(&adsr_envelope);
    final_output = envelope_level * 0.8f;

    test_counter++;
    if (test_counter % 44100 == 0) {
        BSP_LED_Toggle(LED1);
    }

    tx_sample_L = (int16_t)(final_output * 32767.0f);
    tx_sample_R = tx_sample_L;
}

void processMidiPackets() {
    uint8_t *ptr = midiReceiveBuffer;
    uint16_t numPackets = USBH_MIDI_GetLastReceivedDataSize(&hUSBHost) / 4;

    while (numPackets--) {
        ptr++;
        uint8_t type = *ptr++ & 0xF0;
        uint8_t note = *ptr++;
        uint8_t velocity = *ptr++;

        switch(type) {
            case 0x90:
                if(velocity > 0) {
                    if(note_active) {
                        adsr_note_off(&adsr_envelope);
                        note_pending = note;
                        pending_active = 1;
                    } else {
                        Fwave = table_freq[note];
                        current_note = note;
                        note_active = 1;
                        update_filter_cutoff(Fwave);
                        adsr_note_on(&adsr_envelope);
                    }
                } else {
                    if(current_note == note) {
                        note_active = 0;
                        if(pending_active) {
                            Fwave = table_freq[note_pending];
                            current_note = note_pending;
                            note_active = 1;
                            update_filter_cutoff(Fwave);
                            adsr_note_on(&adsr_envelope);
                            pending_active = 0;
                            note_pending = 0;
                        } else {
                            adsr_note_off(&adsr_envelope);
                        }
                    }
                }
                break;

            case 0x80:
                if(current_note == note) {
                    note_active = 0;
                    if(pending_active) {
                        Fwave = table_freq[note_pending];
                        current_note = note_pending;
                        note_active = 1;
                        update_filter_cutoff(Fwave);
                        adsr_note_on(&adsr_envelope);
                        pending_active = 0;
                        note_pending = 0;
                    } else {
                        adsr_note_off(&adsr_envelope);
                    }
                }
                else if(pending_active && note_pending == note) {
                    pending_active = 0;
                    note_pending = 0;
                }
                break;

            case 0xB0:
                if(note == 7) {
                    k = 0.5f + (velocity / 127.0f) * 3.5f;
                    if(note_active && Fwave > 0.0f) {
                        update_filter_cutoff(Fwave);
                    }
                }
                else if(note == 1) {
                    float feedback_amount = (velocity / 127.0f) * 0.85f;
                    reverb_set_feedback(&reverb_left, feedback_amount);
                    reverb_set_feedback(&reverb_right, feedback_amount);
                }
                else if(note == 2) {
                    adsr_envelope.decay_time_ms = 100.0f + (velocity / 127.0f) * 4900.0f;
                    adsr_envelope.decay_decrement = (1.0f - adsr_envelope.sustain_level) / (adsr_envelope.decay_time_ms * adsr_envelope.sample_rate / 1000.0f);
                }
                else if(note == 3) {
                    adsr_envelope.sustain_level = velocity / 127.0f;
                    adsr_envelope.decay_decrement = (1.0f - adsr_envelope.sustain_level) / (adsr_envelope.decay_time_ms * adsr_envelope.sample_rate / 1000.0f);
                    adsr_envelope.release_decrement = adsr_envelope.sustain_level / (adsr_envelope.release_time_ms * adsr_envelope.sample_rate / 1000.0f);
                }
                else if(note == 4) {
                    adsr_envelope.release_time_ms = 100.0f + (velocity / 127.0f) * 4900.0f;
                    adsr_envelope.release_decrement = adsr_envelope.sustain_level / (adsr_envelope.release_time_ms * adsr_envelope.sample_rate / 1000.0f);
                }
                break;

            case 0xE0:
                {
                    uint16_t pitchbend_value = note | (velocity << 7);
                    float pitchbend_normalized = pitchbend_value / 16383.0f;

                    reverb_set_delay_mix(&reverb_left, pitchbend_normalized);
                    reverb_set_delay_mix(&reverb_right, pitchbend_normalized);
                }
                break;
        }
    }
}

void USBH_MIDI_ReceiveCallback(USBH_HandleTypeDef *phost) {
    processMidiPackets();
    USBReceiveAvailable = 1;
}

void init_synthesizer(void) {
    Fwave = 0.0f;
    note_active = 0;
    current_note = 0;
    k = 1.0f;

    note_pending = 0;
    pending_active = 0;

    memset(firState, 0, sizeof(firState));
    arm_fir_init_f32(&fir, N_FILTER, firCoeffs, firState, 1);
    FIR_calc_coeff_f32(&fir, N_FILTER, 0, 1000.0f, 44100.0f, 0);

    adsr_init(&adsr_envelope, 44100);
    reverb_init(&reverb_left);
    reverb_init(&reverb_right);
}

int main(void) {
    HAL_Init();
    MPU_Config();
    CPU_CACHE_Enable();
    SystemClock_Config();

    BSP_LED_Init(LED1);
    BSP_GPIO_Init();
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
    BSP_SDRAM_Init();

    init_synthesizer();

    USBH_Init(&hUSBHost, usbUserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_MIDI_CLASS);
    USBH_Start(&hUSBHost);

    stm32f7_wm8994_init(AUDIO_FREQUENCY_44K,
                       IO_METHOD_INTR,
                       INPUT_DEVICE_INPUT_LINE_1,
                       OUTPUT_DEVICE_HEADPHONE,
                       WM8994_HP_OUT_ANALOG_GAIN_0DB,
                       WM8994_LINE_IN_GAIN_0DB,
                       WM8994_DMIC_GAIN_9DB,
                       0, 0);

    while(1) {
        midiApplication();
        USBH_Process(&hUSBHost);
    }
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

void midiApplication(void) {
    switch (appState) {
    case APP_READY:
        USBH_MIDI_Receive(&hUSBHost, midiReceiveBuffer, MIDI_BUF_SIZE);
        appState = APP_RUNNING;
        break;
    case APP_RUNNING:
        if (USBReceiveAvailable) {
            USBReceiveAvailable = 0;
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



//#include "notes.h"
#include <stdint.h>

// DURÉES ADSR LONGUES POUR OSCILLOSCOPE
#define ATTACK_DURATION  1000.0
#define DECAY_DURATION   1000.0
#define SUSTAIN_LEVEL    0.7
#define RELEASE_DURATION 1000.0

enum state_t { INIT, NOTE_OFF, ATTACK, DECAY, SUSTAIN, RELEASE };

struct adsr_TypeStruct
{
    enum state_t state;
    float current_level;
    float attack_increment;
    float decay_decrement;
    float release_decrement;
    float sustain_level;
    uint32_t sample_rate;
    float attack_time_ms;
    float decay_time_ms;
    float release_time_ms;
};

float adsr(struct adsr_TypeStruct* adsr_s);
void adsr_init(struct adsr_TypeStruct* adsr, uint32_t sample_rate);
void adsr_note_on(struct adsr_TypeStruct* adsr);
void adsr_note_off(struct adsr_TypeStruct* adsr);

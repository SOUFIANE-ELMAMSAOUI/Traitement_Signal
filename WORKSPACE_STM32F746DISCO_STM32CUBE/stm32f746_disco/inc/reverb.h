#ifndef REVERB_H
#define REVERB_H

#include <stdint.h>

#define REVERB_DELAY_MAX 2400
#define REVERB_FEEDBACK_DEFAULT 0.8f
#define REVERB_MIX_DEFAULT 0.9f

struct reverb_TypeStruct {
    float delay_buffer[REVERB_DELAY_MAX];
    uint16_t write_index;
    uint16_t delay_samples;
    float feedback_gain;
    float delay_mix;
};

void reverb_init(struct reverb_TypeStruct* reverb);
float reverb_process(struct reverb_TypeStruct* reverb, float input);
void reverb_set_feedback(struct reverb_TypeStruct* reverb, float feedback);
void reverb_set_delay_mix(struct reverb_TypeStruct* reverb, float delay_mix);

#endif

#include "reverb.h"
#pragma GCC optimize ("O0")

void reverb_init(struct reverb_TypeStruct* reverb) {
    for(int i = 0; i < REVERB_DELAY_MAX; i++) {
        reverb->delay_buffer[i] = 0.0f;
    }

    reverb->write_index = 0;
    reverb->delay_samples = REVERB_DELAY_MAX;
    reverb->feedback_gain = 0.8f;
    reverb->delay_mix = 0.9f;
}

float reverb_process(struct reverb_TypeStruct* reverb, float input) {
    uint16_t read_index = (reverb->write_index - reverb->delay_samples + REVERB_DELAY_MAX) % REVERB_DELAY_MAX;

    float delayed_signal = reverb->delay_buffer[read_index];
    float u = input + (reverb->feedback_gain * delayed_signal);
    
    if(u > 1.0f) u = 1.0f;
    if(u < -1.0f) u = -1.0f;

    reverb->delay_buffer[reverb->write_index] = u;
    
    float delay_output = delayed_signal * reverb->delay_mix;
    float output = input + delay_output;
    
    reverb->write_index = (reverb->write_index + 1) % REVERB_DELAY_MAX;
    
    if(output > 1.0f) output = 1.0f;
    if(output < -1.0f) output = -1.0f;
    
    return output;
}

void reverb_set_feedback(struct reverb_TypeStruct* reverb, float feedback) {
    if(feedback < 0.0f) feedback = 0.0f;
    if(feedback > 0.98f) feedback = 0.98f;
    reverb->feedback_gain = feedback;
}

void reverb_set_delay_mix(struct reverb_TypeStruct* reverb, float delay_mix) {
    if(delay_mix < 0.0f) delay_mix = 0.0f;
    if(delay_mix > 1.0f) delay_mix = 1.0f;
    reverb->delay_mix = delay_mix;
}

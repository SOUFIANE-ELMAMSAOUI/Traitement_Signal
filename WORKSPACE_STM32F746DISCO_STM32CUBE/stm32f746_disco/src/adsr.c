/*
 * adsr.c
 *
 *  Created on: Oct 6, 2022
 *      Author: kerhoas
 */
#include "adsr.h"
#include <stdint.h>
#pragma GCC optimize ("O0")

void adsr_init(struct adsr_TypeStruct* adsr, uint32_t sample_rate) {
    adsr->state = INIT;
    adsr->current_level = 0.0f;
    adsr->sample_rate = sample_rate;

    // Utilise les constantes du .h
    adsr->sustain_level = SUSTAIN_LEVEL;
    adsr->attack_time_ms = ATTACK_DURATION;
    adsr->decay_time_ms = DECAY_DURATION;
    adsr->release_time_ms = RELEASE_DURATION;

    // Calcul des incréments
    adsr->attack_increment = 1.0f / (adsr->attack_time_ms * sample_rate / 1000.0f);
    adsr->decay_decrement = (1.0f - adsr->sustain_level) / (adsr->decay_time_ms * sample_rate / 1000.0f);
    adsr->release_decrement = adsr->sustain_level / (adsr->release_time_ms * sample_rate / 1000.0f);
}

void adsr_note_on(struct adsr_TypeStruct* adsr) {
    adsr->state = ATTACK;
}

void adsr_note_off(struct adsr_TypeStruct* adsr) {
    if (adsr->state != INIT && adsr->state != NOTE_OFF) {
        adsr->state = RELEASE;
    }
}

float adsr(struct adsr_TypeStruct* adsr_s) {

    switch (adsr_s->state) {

        case INIT:
            adsr_s->current_level = 0.0f;
            break;

        case NOTE_OFF:
            adsr_s->current_level = 0.0f;
            break;

        case ATTACK:
            adsr_s->current_level += adsr_s->attack_increment;
            if (adsr_s->current_level >= 1.0f) {
                adsr_s->current_level = 1.0f;
                adsr_s->state = DECAY;
            }
            break;

        case DECAY:
            adsr_s->current_level -= adsr_s->decay_decrement;
            if (adsr_s->current_level <= adsr_s->sustain_level) {
                adsr_s->current_level = adsr_s->sustain_level;
                adsr_s->state = SUSTAIN;
            }
            break;

        case SUSTAIN:
            adsr_s->current_level = adsr_s->sustain_level;
            break;

        case RELEASE:
            adsr_s->current_level -= adsr_s->release_decrement;
            if (adsr_s->current_level <= 0.0f) {
                adsr_s->current_level = 0.0f;
                adsr_s->state = INIT;
            }
            break;
    }
    if (adsr_s->current_level < 0.0f) adsr_s->current_level = 0.0f;
    if (adsr_s->current_level > 1.0f) adsr_s->current_level = 1.0f;

    return adsr_s->current_level;
}

#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "arm_math.h"


typedef struct
  {
    uint16_t numStages;
    float32_t *pState;
    float32_t *coef;
  } arm_iir_instance_f32;

/* FILTER INFORMATION STRUCTURE FOR FILTER ROUTINES */
typedef struct {
    unsigned int length;       /* size of filter */
    float *history;            /* pointer to history in filter */
    float *coef;               /* pointer to coefficients of filter */
} FILTER;

#define FILTER_SECTIONS   2   /* 2 filter sections for 24 db/oct filter */

typedef struct {
        float a0, a1, a2;       /* numerator coefficients */
        float b0, b1, b2;       /* denominator coefficients */
} BIQUAD;

void szxform(
    float *a0, float *a1, float *a2,     /* numerator coefficients */
    float *b0, float *b1, float *b2,   /* denominator coefficients */
    float fc,           /* Filter cutoff frequency */
    float fs,           /* sampling rate */
    float *k,           /* overall gain factor */
    float *coef);         /* pointer to 4 iir coefficients */


void IIR_init_f32( arm_iir_instance_f32 * S,  uint16_t numStages,  float32_t * pCoeffs,  float32_t * pState,  uint32_t blockSize);
int IIR_filt_f32(arm_iir_instance_f32 *S, float* x, float* y);
void IIR_calc_coeff_f32(arm_iir_instance_f32 *S, uint16_t numStages,float32_t fc1, float32_t fc2, float32_t fe, int filter_type );

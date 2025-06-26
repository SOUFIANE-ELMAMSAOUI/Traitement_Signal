#include "IIR_filter.h"
#pragma GCC optimize ("O0")

//=====================================================================
void IIR_calc_coeff_f32(arm_iir_instance_f32 *S, uint16_t numStages,float32_t fc1, float32_t fc2, float32_t fe, int filter_type )
{
 // A COMPLETER
}
//=====================================================================
void IIR_init_f32( arm_iir_instance_f32 * S,  uint16_t numStages,  float32_t * pCoeffs,  float32_t * pState,  uint32_t blockSize)
{
	  S->numStages = numStages;
	  S->coef = pCoeffs;
	  S->pState = pState;
}
//=====================================================================
int IIR_filt_f32(arm_iir_instance_f32 *S, float* x, float* y)
{
    unsigned int i;
    float *wn_1_ptr,*wn_2_ptr,*coef_ptr;
    float output=0.0;
    float wn,wn_1,wn_2;

    coef_ptr = S->coef;                /* coefficient pointer */

    wn_1_ptr = S->pState;            /* first history */
    wn_2_ptr = wn_1_ptr + 1;           /* next history */

    output = *x * (*coef_ptr++);

    for (i = 0 ; i < S->numStages; i++)
        {
        wn_1 = *wn_1_ptr;           /* history values */
        wn_2 = *wn_2_ptr;

        output = output - wn_1 * (*coef_ptr++);
        wn = output - wn_2 * (*coef_ptr++);    /* poles */

        output = wn + wn_1 * (*coef_ptr++);
        output = output + wn_2 * (*coef_ptr++);      /* zeros */

        *wn_2_ptr++ = *wn_1_ptr;
        *wn_1_ptr++ = wn;
        wn_1_ptr++;
        wn_2_ptr++;
    }
    *y = output;
    return 0;
}
//=====================================================================





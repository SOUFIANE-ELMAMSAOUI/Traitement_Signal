/*
 * FIR_filters.h
 *
 *  Created on: 13 sept. 2016
 *      Author: kerhoas
 */

#ifndef INC_FIR_FILTERS_H_
#define INC_FIR_FILTERS_H_

#include "arm_math.h"

void FIR_init_f32(arm_fir_instance_f32 *S, uint16_t numTaps,float32_t *pCoeffs, float32_t *pState );
void FIR_calc_coeff_f32(arm_fir_instance_f32 *S, uint16_t numTaps,float32_t fc1, float32_t fc2, float32_t fe, int filter_type );
void FIR_custom_init_f32( arm_fir_instance_f32 *S, uint16_t numTaps,float32_t fc1, float32_t fc2, float32_t fe,  float32_t *pCoeffs, float32_t *pState );


void FIR_filt_f32(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst);

void FIR_filt_f32_circular(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst);

void FIR_init_q15(arm_fir_instance_q15 *S, uint16_t numTaps, q15_t * pCoeffs, q15_t * pState);
void FIR_filt_q15(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst);
void FIR_filt_q15_circular(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst);

#endif /* INC_FIR_FILTERS_H_ */

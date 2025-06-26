/*
 * FIR_filterc.c
 *
 *  Created on: 13 sept. 2016
 *      Author: kerhoas
 */

#include <FIR_filter.h>


//=======================================================================================
void FIR_calc_coeff_f32(arm_fir_instance_f32 *S, uint16_t numTaps,float32_t fc1, float32_t fc2, float32_t fe, int filter_type )
{
	float32_t *pCoeffs = S->pCoeffs;
	    const float32_t nyquist = fe / 2.0f;
	    const float32_t norm_cutoff = fc2 / nyquist; // Normalisation [0, 1]
	    const int center = numTaps / 2;             // Coefficient central

	    // 1. Calcul de la réponse impulsionnelle idéale (fonction sinc)
	    for(int n = 0; n < numTaps; n++)
	    {
	        if(n == center)
	        {
	            // Cas spécial pour éviter la division par 0
	            pCoeffs[n] = 2.0f * norm_cutoff;
	        }
	        else
	        {
	            float32_t t = M_PI * (n - center);
	            pCoeffs[n] = sinf(2.0f * M_PI * norm_cutoff * (n - center)) / t;
	        }
	    }

	    // 2. Application de la fenêtre de Hamming (réduit les ondulations)
	    for(int n = 0; n < numTaps; n++)
	    {
	        const float32_t window = 0.54f - 0.46f * cosf(2.0f * M_PI * n / (numTaps - 1));
	        pCoeffs[n] *= window;
	    }

	    // 3. Normalisation pour un gain unitaire en bande passante
	    float32_t sum = 0.0f;
	    for(int n = 0; n < numTaps; n++) sum += pCoeffs[n];
	    for(int n = 0; n < numTaps; n++) pCoeffs[n] /= sum;

}
//=======================================================================================

void FIR_init_f32( arm_fir_instance_f32 *S, uint16_t numTaps,float32_t *pCoeffs, float32_t *pState )
{
  uint16_t i;

  S->numTaps = numTaps;
  S->pCoeffs = pCoeffs;
  S->pState = pState;
  for (i = 0; i < S->numTaps; i++)
  {
    S->pState[i] = 0;
  }
}

//=======================================================================================
void FIR_filt_f32(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst) {

	uint16_t i;
	float32_t acc;

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[0] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += S->pState[i]*S->pCoeffs[i];
    }
	*pDst = acc;

	// Vieillissement des échantillons d'entrée
    for (i = S->numTaps-1; i > 0; i--)
    {
      S->pState[i] = S->pState[i-1];
    }
}

//=======================================================================================

void FIR_filt_f32_circular(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst) {

	int16_t i;
	float32_t acc;
	static int16_t k=0;	// younger sample index

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[k] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += S->pState[k++]*S->pCoeffs[i];
       if (k >= S->numTaps) k = 0;

    }
	*pDst = acc;

	k--;
	if (k <0) k = S->numTaps-1;
}

//=======================================================================================

void FIR_init_q15(arm_fir_instance_q15 *S, uint16_t numTaps, q15_t * pCoeffs, q15_t * pState)
{
	  uint16_t i;
	  S->numTaps = numTaps;
	  S->pCoeffs = pCoeffs;
	  S->pState = pState;
	  for (i = 0; i < S->numTaps; i++)
	  {
	    S->pState[i] = 0;
	  }
}

//=======================================================================================

void FIR_filt_q15(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst) {

	uint16_t i;
	int32_t acc;

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[0] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += (int32_t) S->pState[i]*S->pCoeffs[i];
    }
	*pDst = (int16_t) (acc >> 16);

	// Vieillissement des échantillons d'entrée
    for (i = S->numTaps-1; i > 0; i--)
    {
      S->pState[i] = S->pState[i-1];
    }
}

//=======================================================================================

void FIR_filt_q15_circular(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst) {

	uint16_t i;
	int32_t acc;
	static int16_t k=0;	// younger sample index

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[k] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += (int32_t) S->pState[k++]*S->pCoeffs[i];
       if (k >= S->numTaps) k = 0;
    }
	*pDst = (int16_t) (acc >> 16);

	//k++;
	//if (k >= S->numTaps) k = 0;
	k--;
	if (k <0) k = S->numTaps-1;
}

//=======================================================================================


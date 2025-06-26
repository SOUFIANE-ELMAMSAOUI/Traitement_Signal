/*
 * ----------------------------------------------------------
 *      Pre-warp the coefficients of a numerator or denominator.
 *      Note that a0 is assumed to be 1, so there is no wrapping
 *      of it.
 * ----------------------------------------------------------
 */
#include <math.h>
#pragma GCC optimize ("O0")

void prewarp(
    float *a0, float *a1, float *a2,
	float fc, float fs)
{
	float wp, pi;

    pi = 4.0 * atan(1.0);
    wp = 2.0 * fs * tan(pi * fc / fs);

    *a2 = (*a2) / (wp * wp);
    *a1 = (*a1) / wp;
}


/*
 * ----------------------------------------------------------
 * bilinear()
 *
 * Transform the numerator and denominator coefficients
 * of s-domain biquad section into corresponding
 * z-domain coefficients.
 *
 *      Store the 4 IIR coefficients in array pointed by coef
 *      in following order:
 *             beta1, beta2    (denominator)
 *             alpha1, alpha2  (numerator)
 *
 * Arguments:
 *             a0-a2   - s-domain numerator coefficients
 *             b0-b2   - s-domain denominator coefficients
 *             k               - filter gain factor. initially set to 1
 *                                and modified by each biquad section in such
 *                                a way, as to make it the coefficient by
 *                                which to multiply the overall filter gain
 *                                in order to achieve a desired overall filter
gain,
 *                                specified in initial value of k.
 *             fs             - sampling rate (Hz)
 *             coef    - array of z-domain coefficients to be filled in.
 *
 * Return:
 *             On return, set coef z-domain coefficients
 * ----------------------------------------------------------
 */
void bilinear(
    float a0, float a1, float a2,    /* numerator coefficients */
	float b0, float b1, float b2,    /* denominator coefficients */
	float *k,           /* overall gain factor */
	float fs,           /* sampling rate */
    float *coef         /* pointer to 4 iir coefficients */
)
{
	float ad, bd;

                 /* alpha (Numerator in s-domain) */
    ad = 4. * a2 * fs * fs + 2. * a1 * fs + a0;
                 /* beta (Denominator in s-domain) */
    bd = 4. * b2 * fs * fs + 2. * b1* fs + b0;

                 /* update gain constant for this section */
    *k *= ad/bd;

                 /* Denominator */
    *coef++ = (2. * b0 - 8. * b2 * fs * fs)
                           / bd;         /* beta1 */
    *coef++ = (4. * b2 * fs * fs - 2. * b1 * fs + b0)
                           / bd; /* beta2 */

                 /* Nominator */
    *coef++ = (2. * a0 - 8. * a2 * fs * fs)
                           / ad;         /* alpha1 */
    *coef = (4. * a2 * fs * fs - 2. * a1 * fs + a0)
                           / ad;   /* alpha2 */
}


/*
 * ----------------------------------------------------------
 * Transform from s to z domain using bilinear transform
 * with prewarp.
 *
 * Arguments:
 *      For argument description look at bilinear()
 *
 *      coef - pointer to array of floating point coefficients,
 *                     corresponding to output of bilinear transofrm
 *                     (z domain).
 *
 * Note: frequencies are in Hz.
 * ----------------------------------------------------------
 */
void szxform(
		float *a0, float *a1, float *a2, /* numerator coefficients */
		float *b0, float *b1, float *b2, /* denominator coefficients */
		float fc,         /* Filter cutoff frequency */
		float fs,         /* sampling rate */
		float *k,         /* overall gain factor */
    float *coef)         /* pointer to 4 iir coefficients */
{
                 /* Calculate a1 and a2 and overwrite the original values */
        prewarp(a0, a1, a2, fc, fs);
        prewarp(b0, b1, b2, fc, fs);
        bilinear(*a0, *a1, *a2, *b0, *b1, *b2, k, fs, coef);
}


// ----------------- file bilinear.c end -----------------

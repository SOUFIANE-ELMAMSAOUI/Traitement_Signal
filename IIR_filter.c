// IIR_filter.c - Version corrigée
#include "IIR_filter.h"
#include "arm_math.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#pragma GCC optimize ("O0")

static float32_t tab_history[4] = {0};

//=====================================================================
void IIR_calc_coeff_f32(arm_iir_instance_f32 *S,
                        uint16_t numStages,
                        float32_t fc1,
                        float32_t fc2,
                        float32_t fe,
                        int filter_type,
                        float Q)
{
    // Vérification des paramètres
    if (S == NULL || numStages != 2 || fe <= 0 || Q <= 0.0f) {
        return;
    }

    // Limitation de la fréquence de coupure
    if (fc1 <= 0) fc1 = 50.0f;
    if (fc1 >= fe/2.2f) fc1 = fe/2.2f;  // Marge de sécurité plus grande

    float32_t *coeffs = S->coef;

    // Fréquence normalisée avec limitation
    float32_t omega = 2.0f * M_PI * fc1 / fe;
    if (omega >= M_PI * 0.95f) omega = M_PI * 0.95f;  // Sécurité

    float32_t cos_omega = cosf(omega);
    float32_t sin_omega = sinf(omega);

    // Limitation du Q pour éviter l'instabilité
    if (Q > 10.0f) Q = 10.0f;
    if (Q < 0.1f) Q = 0.1f;

    // Pour un filtre passe-bas Butterworth d'ordre 4
    // Section 1: Q = 1/(2*cos(π/8)) ≈ 0.541
    float32_t Q1 = 0.541f * Q;  // Modulé par le paramètre Q global
    float32_t alpha1 = sin_omega / (2.0f * Q1);

    // Coefficients section 1 (format Direct Form II)
    float32_t a0_1 = 1.0f + alpha1;
    coeffs[0] = (1.0f - cos_omega) / (2.0f * a0_1); // b0
    coeffs[1] = (1.0f - cos_omega) / a0_1;           // b1
    coeffs[2] = (1.0f - cos_omega) / (2.0f * a0_1); // b2
    coeffs[3] = (-2.0f * cos_omega) / a0_1;          // -a1
    coeffs[4] = (1.0f - alpha1) / a0_1;              // -a2

    // Section 2: Q = 1/(2*cos(3π/8)) ≈ 1.307
    float32_t Q2 = 1.307f * Q;  // Modulé par le paramètre Q global
    float32_t alpha2 = sin_omega / (2.0f * Q2);

    // Coefficients section 2
    float32_t a0_2 = 1.0f + alpha2;
    coeffs[5] = (1.0f - cos_omega) / (2.0f * a0_2); // b0
    coeffs[6] = (1.0f - cos_omega) / a0_2;           // b1
    coeffs[7] = (1.0f - cos_omega) / (2.0f * a0_2); // b2
    coeffs[8] = (-2.0f * cos_omega) / a0_2;          // -a1
    coeffs[9] = (1.0f - alpha2) / a0_2;              // -a2

    S->numStages = numStages;
}

//=====================================================================
void IIR_init_f32(arm_iir_instance_f32 * S, uint16_t numStages, float32_t * pCoeffs, float32_t * pState, uint32_t blockSize)
{
    S->numStages = numStages;
    S->coef = pCoeffs;
    S->pState = pState;
}

//=====================================================================
// Version avec protection contre les valeurs aberrantes
void IIR_filter_f32_optimized(arm_iir_instance_f32 *S, float32_t *input, float32_t *output, uint32_t blockSize)
{
    float32_t *coef = S->coef;
    float32_t *pState = S->pState;

    for(uint32_t n = 0; n < blockSize; n++) {
        float32_t xn = input[n];

        // Protection contre les valeurs aberrantes en entrée
        if (xn > 2.0f) xn = 2.0f;
        if (xn < -2.0f) xn = -2.0f;

        // Section 1
        float32_t w1 = xn - coef[3] * pState[0] - coef[4] * pState[1];
        float32_t y1 = coef[0] * w1 + coef[1] * pState[0] + coef[2] * pState[1];

        // Protection contre l'explosion numérique
        if (fabsf(w1) > 10.0f) w1 = (w1 > 0) ? 10.0f : -10.0f;
        if (fabsf(y1) > 10.0f) y1 = (y1 > 0) ? 10.0f : -10.0f;

        // Mise à jour état section 1
        pState[1] = pState[0];
        pState[0] = w1;

        // Section 2 (entrée = sortie section 1)
        float32_t w2 = y1 - coef[8] * pState[2] - coef[9] * pState[3];
        float32_t y2 = coef[5] * w2 + coef[6] * pState[2] + coef[7] * pState[3];

        // Protection contre l'explosion numérique
        if (fabsf(w2) > 10.0f) w2 = (w2 > 0) ? 10.0f : -10.0f;
        if (fabsf(y2) > 10.0f) y2 = (y2 > 0) ? 10.0f : -10.0f;

        // Mise à jour état section 2
        pState[3] = pState[2];
        pState[2] = w2;

        output[n] = y2;
    }
}

//=====================================================================
// Fonction pour réinitialiser les états du filtre (utile en cas de problème)
void IIR_reset_states(arm_iir_instance_f32 *S)
{
    for(int i = 0; i < 4; i++) {
        S->pState[i] = 0.0f;
    }
}

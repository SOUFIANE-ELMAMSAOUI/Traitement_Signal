#ifndef IIR_H_
#define IIR_H_
#include <math.h>

typedef struct {
  float fc; // Filter cutoff frequency
  float Q; // Filter resonance
  float alpha1, alpha2; // Filter coefficients
  float yb1, yl1; // Filter state variables
  float yl;
} state_variable_filter_t;

void SVF_init(state_variable_filter_t *f, float fe, float cutoff, float resonance);
void SVF_process(state_variable_filter_t *f,float x);








#endif /* IIR_H_ */



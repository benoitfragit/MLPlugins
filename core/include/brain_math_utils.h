#ifndef BRAIN_MATH_UTILS_H
#define BRAIN_MATH_UTILS_H
#include "brain_core_types.h"
#include <math.h>
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define EPSILON 1e-6
#define BRAIN_POSITIVE_SGN 1
#define BRAIN_NEGATIVE_SGN -1
#define BRAIN_SGN(val) ((val>0)?BRAIN_POSITIVE_SGN:((val<0)?BRAIN_NEGATIVE_SGN:0))
/**********************************************************************/
/**                        ACTIVATION FUNCTIONS                      **/
/**********************************************************************/
BrainReal identity(const BrainReal value);
BrainReal sigmoid(const BrainReal value);
BrainReal tangeant_hyperbolic(const BrainReal value);
BrainReal co_tangeant(const BrainReal value);
BrainReal softplus(const BrainReal value);
BrainReal sinusoid(const BrainReal value);
/**********************************************************************/
/**                        DERIVATIVE FUNCTIONS                      **/
/**********************************************************************/
BrainReal identity_derivative(const BrainReal value);
BrainReal sigmoid_derivative(const BrainReal value);
BrainReal tangeant_hyperbolic_derivative(const BrainReal value);
BrainReal co_tangeant_derivative(const BrainReal value);
BrainReal softplus_derivative(const BrainReal value);
BrainReal sinusoid_derivative(const BrainReal value);
/**********************************************************************/
/**                           COST FUNCTIONS                         **/
/**********************************************************************/
BrainReal quadratic_cost(const BrainReal output, const BrainReal desired);
BrainReal crossentropy_cost(const BrainReal output, const BrainReal desired);
/**********************************************************************/
/**                       COST FUNCTION DERIVATIVE                   **/
/**********************************************************************/
BrainReal quadratic_cost_derivative(const BrainReal output, const BrainReal desired);
BrainReal crossentropy_cost_derivative(const BrainReal output, const BrainReal desired);
/**********************************************************************/
/**                         VECTOR FUNCTIONS                         **/
/**********************************************************************/
BrainReal dot(const BrainReal* a, const BrainReal* b, const BrainUint size);
BrainReal distance(const BrainReal* a, const BrainReal* b, const BrainUint size);
BrainReal norm2(const BrainReal* a, const BrainUint size);
void normalization(BrainReal** signals, BrainReal* means, BrainReal* sigmas, const BrainUint number_of_signals, const BrainUint size);
void kmeans(BrainReal** signals, BrainReal** centers, const BrainUint number_of_class, const BrainUint number_of_signals, const BrainUint size);
#endif /* BRAIN_MATH_UTILS_H */

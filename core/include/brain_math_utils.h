#ifndef BRAIN_MATH_UTILS_H
#define BRAIN_MATH_UTILS_H
#include "brain_core_types.h"
#include <math.h>
/**
 * \def MIN(a,b)
 */
#define MIN(a, b) ((a) < (b) ? (a) : (b))
/**
 * \def MAX(a,b)
 */
#define MAX(a, b) ((a) < (b) ? (b) : (a))
/**
 * \def EPSILON
 */
#define EPSILON 1e-6
/**
 * \def BRAIN_POSITIVE_SGN
 */
#define BRAIN_POSITIVE_SGN 1
/**
 * \def BRAIN_NEGATIVE_SGN
 */
#define BRAIN_NEGATIVE_SGN -1
/**
 * \def BRAIN_SGN(val)
 */
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
BrainReal relu(const BrainReal value);
/**********************************************************************/
/**                        DERIVATIVE FUNCTIONS                      **/
/**********************************************************************/
BrainReal identity_derivative(const BrainReal value);
BrainReal sigmoid_derivative(const BrainReal value);
BrainReal tangeant_hyperbolic_derivative(const BrainReal value);
BrainReal co_tangeant_derivative(const BrainReal value);
BrainReal softplus_derivative(const BrainReal value);
BrainReal sinusoid_derivative(const BrainReal value);
BrainReal relu_derivative(const BrainReal value);
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
#endif /* BRAIN_MATH_UTILS_H */

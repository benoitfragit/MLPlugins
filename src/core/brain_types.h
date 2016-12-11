#ifndef BRAIN_TYPES_H
#define BRAIN_TYPES_H

#define BRAIN_TRUE  1
#define BRAIN_FALSE 0

typedef unsigned char BrainBool;
typedef int           BrainInt;
typedef unsigned int  BrainUint;
typedef float         BrainFloat;
typedef double        BrainDouble;
typedef const char*   BrainString;
typedef char          BrainChar;
typedef BrainDouble*  BrainSignal;
typedef BrainDouble*  BrainWeight;

typedef enum BrainResult
{
    BRAIN_FAILED  = 0,
    BRAIN_SUCCESS = 1
} BrainResult;

typedef enum BrainActivationType
{
    Invalid_Activation,
    Identity,
    Sigmoid,
    TanH,
    ArcTan,
    SoftPlus,
    Sinusoid,

    First_Activation = Invalid_Activation,
    Last_Activation  = Sinusoid
} BrainActivationType;

typedef enum BrainCostFunctionType
{
    Invalid_CostFunction,
    Quadratic,
    CrossEntropy,

    First_CostFunction = Invalid_CostFunction,
    Last_CostFunction  = CrossEntropy
} BrainCostFunctionType;

typedef BrainDouble (*PtrFunc)(const BrainDouble value);
typedef BrainDouble (*CostPtrFunc)(const BrainDouble output,
                                   const BrainDouble desired);
typedef struct Data*    BrainData;
typedef struct Neuron*  BrainNeuron;
typedef struct Layer*   BrainLayer;
typedef struct Network* BrainNetwork;

#endif /* BRAIN_TYPES_H */

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

typedef enum BrainResult
{
    BRAIN_FAILED  = 0,
    BRAIN_SUCCESS = 1
} BrainResult;

typedef enum BrainActivationType
{
    Invalid,
    Identity,
    Sigmoid,
    TanH,
    ArcTan,
    SoftPlus,
    Sinusoid,

    First_Activation = Invalid,
    Last_Activation = Sinusoid
} BrainActivationType;

typedef BrainDouble (*PtrFunc)(const BrainDouble value);

typedef struct Neuron*  BrainNeuron;
typedef struct Layer*   BrainLayer;
typedef struct Synapse* BrainSynapse;
typedef struct Network* BrainNetwork;

#endif /* BRAIN_TYPES_H */

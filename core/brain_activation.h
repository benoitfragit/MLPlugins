#ifndef BRAIN_ACTIVATION_H
#define BRAIN_ACTIVATION_H

typedef enum ActivationType
{
    Identity,
    Sigmoid,
    TanH,
    ArcTan,
    SoftPlus,
    Sinusoid,

    First = Identity,
    Last  = Sinusoid
} ActivationType;

typedef double (*PtrFunc)(const double value);

PtrFunc activation(const ActivationType type);
PtrFunc derivative(const ActivationType type);

#endif /* BRAIN_ACTIVATION_H */

#ifndef BRAIN_ACTIVATION_H
#define BRAIN_ACTIVATION_H

typedef enum ActivationType
{
    Invalid,
    Identity,
    Sigmoid,
    TanH,
    ArcTan,
    SoftPlus,
    Sinusoid,

    First_Activation = Invalid,
    Last_Activation  = Sinusoid
} ActivationType;

typedef double (*PtrFunc)(const double value);

PtrFunc activation(const ActivationType type);
PtrFunc derivative(const ActivationType type);
ActivationType get_activation_type(const char* activation_type_name);
const char* get_activation_name(const ActivationType activation_type);

#endif /* BRAIN_ACTIVATION_H */

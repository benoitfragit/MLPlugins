#ifndef BRAIN_SIGNAL_UTILS_H
#define BRAIN_SIGNAL_UTILS_H

#include "brain_core_types.h"

BrainReal dot(const BrainReal* a,
              const BrainReal* b,
              const BrainUint size);

BrainReal distance( const BrainReal* a,
                    const BrainReal* b,
                    const BrainUint size);

BrainReal norm2(const BrainReal* a,
                const BrainUint size);

void FindGaussianModel( BrainReal** signals,
                        BrainReal* means,
                        BrainReal* sigmas,
                        const BrainUint number_of_signals,
                        const BrainUint size);

void ApplyGaussianModel(BrainReal** signals,
                        BrainReal* means,
                        BrainReal* sigmas,
                        const BrainUint number_of_signals,
                        const BrainUint size);

void FindMinMaxModel(BrainReal** signals,
                     BrainReal* min,
                     BrainReal* max,
                     const BrainUint number_of_signals,
                     const BrainUint size);

void ApplyMinMaxModel(  BrainReal** signals,
                        BrainReal* min,
                        BrainReal* max,
                        const BrainUint number_of_signals,
                        const BrainUint size);

void kmeans(BrainReal** signals,
            BrainReal** centers,
            BrainUint* labels,
            const BrainUint number_of_class,
            const BrainUint number_of_signals,
            const BrainUint size);

#endif /* BRAIN_SIGNAL_UTILS_H */

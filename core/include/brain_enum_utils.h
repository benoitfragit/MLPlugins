#ifndef BRAIN_ENUM_UTILS_H
#define BRAIN_ENUM_UTILS_H
#include "brain_core_types.h"

BrainUint get_enum_values(const BrainString* labels,
			  const BrainUint first,
			  const BrainUint last,
			  BrainString val);

#endif /* BRAIN_ENUM_UTILS_H */

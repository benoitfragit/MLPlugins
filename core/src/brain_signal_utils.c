#include "brain_signal_utils.h"
#include "brain_memory_utils.h"
#include "brain_random_utils.h"
#include "brain_math_utils.h"

#include <math.h>

BrainReal
dot(const BrainReal* a, const BrainReal* b, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        BRAIN_ALLOCATED(b) &&
        (size > 0))
    {
        BrainUint i = 0;
        for (i = 0; i < size; ++i)
        {
            ret += a[i] * b[i];
        }
    }

    return ret;
}

BrainReal
distance(const BrainReal* a, const BrainReal* b, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        BRAIN_ALLOCATED(b) &&
        (size > 0))
    {
        BrainUint i = 0;
        BrainReal t = 0.;
        for (i = 0; i < size; ++i)
        {
            t = a[i] - b[i];
            ret += t*t;
        }

        ret = sqrt(ret);
    }

    return ret;
}

BrainReal
norm2(const BrainReal* a, const BrainUint size)
{
    BrainReal ret = 0.;

    if (BRAIN_ALLOCATED(a) &&
        (size > 0))
    {
        BrainUint i = 0;
        for (i = 0; i < size; ++i)
        {
            ret += a[i]*a[i];
        }

        ret = sqrt(ret);
    }

    return ret;
}

void
FindGaussianModel(BrainReal** signals,
                      BrainReal* means,
                      BrainReal* sigmas,
                      const BrainUint number_of_signals,
                      const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals)
    &&  BRAIN_ALLOCATED(means)
    &&  BRAIN_ALLOCATED(sigmas)
    &&  (0 < size)
    &&  (number_of_signals))
    {
        BrainUint i = 0;
        BrainUint j = 0;
        /**************************************************************/
        /**                       GET THE MEAN                       **/
        /**************************************************************/
        for (j = 0; j < size; ++j)
        {
            means[j] = 0.;
            for (i = 0; i < number_of_signals; ++i)
            {
                if (BRAIN_ALLOCATED(signals[i]))
                {
                    means[j] += signals[i][j];
                }
            }
            means[j] /= (BrainReal)number_of_signals;
        }
        /**************************************************************/
        /**                      GET THE VARIANCE                    **/
        /**************************************************************/
        for (j = 0; j < size; ++j)
        {
            sigmas[j] = 0.;
            for (i = 0; i < number_of_signals; ++i)
            {
                if (BRAIN_ALLOCATED(signals[i]))
                {
                    const BrainReal diff = signals[i][j] - means[j];
                    sigmas[j] += diff * diff;
                }
            }
            sigmas[j] /= (BrainReal)number_of_signals;
        }
    }
}

void
ApplyGaussianModel(BrainReal** signals,
                   BrainReal* means,
                   BrainReal* sigmas,
                   const BrainUint number_of_signals,
                   const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals)
    &&  BRAIN_ALLOCATED(sigmas)
    &&  BRAIN_ALLOCATED(means))
    {
        BrainUint i;
        BrainUint j;
        /**************************************************************/
        /**                  CENTER AND SCALE EACH VECTOR            **/
        /**************************************************************/
        for (i = 0; i < number_of_signals; ++i)
        {
            if (BRAIN_ALLOCATED(signals[i]))
            {
                for (j = 0;j < size; ++j)
                {
                    signals[i][j] = (signals[i][j] - means[j]) / sqrt(sigmas[j]);
                }
            }
        }
    }
}

void FindMinMaxModel(BrainReal** signals,
                     BrainReal* min,
                     BrainReal* max,
                     const BrainUint number_of_signals,
                     const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals)
    &&  (0 < number_of_signals)
    &&  BRAIN_ALLOCATED(min)
    &&  BRAIN_ALLOCATED(max))
    {
        BrainUint i = 0;
        BrainUint j = 0;

        BRAIN_COPY(signals[0], min, BrainReal, size);
        BRAIN_COPY(signals[0], max, BrainReal, size);

        for (i = 0; i < number_of_signals; ++i)
        {
            for (j = 0; j< size; ++j)
            {
                min[j] = MIN(min[j], signals[i][j]);
                max[j] = MAX(max[j], signals[i][j]);
            }
        }
    }
}

void
ApplyMinMaxModel(BrainReal** signals,
                 BrainReal* min,
                 BrainReal* max,
                 const BrainUint number_of_signals,
                 const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals)
    &&  (0 < number_of_signals)
    &&  (0 < size)
    &&  BRAIN_ALLOCATED(min)
    &&  BRAIN_ALLOCATED(max))
    {
        BrainUint i = 0;
        BrainUint j = 0;

        for (i = 0; i < number_of_signals; ++i)
        {
            for (j = 0; j < size; ++j)
            {
                signals[i][j] = signals[i][j] - (max[j] + min[j]) / 2.0;
                if (min[j] < max[j])
                {
                    signals[i][j] /= (max[j] - min[j]);
                }
            }
        }
    }
}

void
kmeans(BrainReal** signals,
       BrainReal** centers,
       BrainUint* labels,
       const BrainUint number_of_class,
       const BrainUint number_of_signals,
       const BrainUint size)
{
    if (BRAIN_ALLOCATED(signals) &&
        BRAIN_ALLOCATED(centers) &&
        BRAIN_ALLOCATED(labels))
    {
        BrainUint i = 0;
        BrainUint j = 0;
        BrainUint k = 0;
        BrainUint* tmp = NULL;
        BrainUint new_index = 0;
        BrainBool changed = BRAIN_TRUE;
        /**************************************************************/
        /**                    RANDOMLY SELECT K CENTROIDS           **/
        /**************************************************************/
        BRAIN_NEW(tmp, BrainUint, number_of_class);
        for (i = 0; i < number_of_class; ++i)
        {
            BrainBool valid = BRAIN_TRUE;
            do
            {
                new_index = BRAIN_RAND_RANGE(0,number_of_signals);

                for (j = 0; j < number_of_class; ++j)
                {
                    if (tmp[j] == new_index)
                    {
                        valid = BRAIN_FALSE;
                        break;
                    }
                }
            } while (!valid);
            tmp[i] = new_index;
            BRAIN_COPY(signals[new_index], centers[i], BrainReal, size);
        }
        BRAIN_DELETE(tmp);
        /**************************************************************/
        /**              UPDATE ALL CENTROIDS AND LABELS             **/
        /**************************************************************/
        while (changed)
        {
            changed = BRAIN_FALSE;
            /**********************************************************/
            /**                  RECOMPUTE ALL LABELS                **/
            /**********************************************************/
            for (i = 0; i < number_of_signals; ++i)
            {
                BrainReal dist = distance(signals[i], centers[0], size);
                BrainUint label = 0;
                for (j = 1; j < number_of_class; ++j)
                {
                    const BrainReal new_dist = distance(signals[i], centers[j], size);
                    if (new_dist < dist)
                    {
                        dist = new_dist;
                        label = j;
                    }
                }

                if (label != labels[i])
                {
                    labels[i] = label;
                    changed = BRAIN_TRUE;
                }
            }
            /**********************************************************/
            /**                RECOMPUTE ALL CENTROIDS               **/
            /**********************************************************/
            if(changed)
            {
                for (i = 0; i < number_of_class; ++i)
                {
                    BrainUint num = 0;
                    BRAIN_SET(centers[i], 0, BrainReal, number_of_class);
                    /******************************************************/
                    /**              CENTROIDS CALCULATION               **/
                    /******************************************************/
                    for (j = 0; j < number_of_signals; ++j)
                    {
                        if (labels[j] == i)
                        {
                            ++num;
                            for (k = 0; k < size; ++k)
                            {
                                centers[i][k] += signals[j][k];
                            }
                        }
                    }
                    /******************************************************/
                    /**               CENTROIDS AVERAGING                **/
                    /******************************************************/
                    for (k = 0; k < size; ++k)
                    {
                        if (0 < num)
                        {
                            centers[i][k] /= (BrainReal)num;
                        }
                    }
                }
            }
        }
    }
}

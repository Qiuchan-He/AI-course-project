#include "q_learning/q_learning.h"

#include "q_learning/experience_sample.h"
#include "q_learning/experience_sampler.h"
#include "q_learning/qvf_approximator.h"

#include "downward/utils/system.h"

namespace q_learning {

void run_q_learning(
    ExperienceSampler& sampler,
    QVFApproximator& qvf,
    double discount_factor,
    double learning_rate)
{
   while(auto sample = sampler.sample_experience(qvf)) {
       qvf.update(*sample, discount_factor, learning_rate);
   }
}

} // namespace q_learning

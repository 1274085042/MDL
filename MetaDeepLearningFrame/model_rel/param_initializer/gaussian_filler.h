#pragma once

#include "./facilities/policies.h"
#include "./facilities/fill_with_dist.h"
#include "../../policies/policy_container.h"
#include <exception>

namespace MDL
{
    template<typename tPolicyCont = PolicyContainer<>>
    class GaussianFiller
    {
    private:
        using RandomEngine = InitPolicy::RandEngine;
        RandomEngine re_;                           //随机数引擎
        double mean_;
        double std_;

    public:
        GaussianFiller(const double &pMean, const double &pStd):mean_(pMean), std_(pStd), re_(std::random_device{}())
        {
            if(std_<=0)
            {
                throw std::runtime_error("Invalid std");
            }
        }

        template<typename tData>
        void Fill(tData &data)
        {
            using ElementType = typename tData::ElementType;
            std::normal_distribution<ElementType> dis(mean_, std_);
            NSInitializer::Fill(data, dis, re_);
        }
    };
}
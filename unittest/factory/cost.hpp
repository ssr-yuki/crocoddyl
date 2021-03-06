///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2018-2020, LAAS-CNRS, University of Edinburgh
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef CROCODDYL_COST_FACTORY_HPP_
#define CROCODDYL_COST_FACTORY_HPP_

#include "state.hpp"
#include "activation.hpp"
#include "crocoddyl/multibody/cost-base.hpp"
#include "crocoddyl/multibody/numdiff/cost.hpp"
#include "crocoddyl/multibody/costs/cost-sum.hpp"

namespace crocoddyl {
namespace unittest {

struct CostModelTypes {
  enum Type {
    CostModelState,
    CostModelControl,
    CostModelCoMPosition,
    // CostModelCentroidalMomentum,  // @todo Figure out the pinocchio callbacks.
    CostModelFramePlacement,
    CostModelFrameRotation,
    CostModelFrameTranslation,
    CostModelFrameVelocity,
    NbCostModelTypes
  };
  static std::vector<Type> init_all() {
    std::vector<Type> v;
    v.clear();
    for (int i = 0; i < NbCostModelTypes; ++i) {
      v.push_back((Type)i);
    }
    return v;
  }
  static const std::vector<Type> all;
};

std::ostream& operator<<(std::ostream& os, CostModelTypes::Type type);

class CostModelFactory {
 public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  typedef crocoddyl::MathBaseTpl<double> MathBase;
  typedef typename MathBase::Vector6s Vector6d;

  explicit CostModelFactory();
  ~CostModelFactory();

  boost::shared_ptr<crocoddyl::CostModelAbstract> create(
      CostModelTypes::Type cost_type, StateModelTypes::Type state_type, ActivationModelTypes::Type activation_type,
      std::size_t nu = std::numeric_limits<std::size_t>::max()) const;
};

boost::shared_ptr<crocoddyl::CostModelAbstract> create_random_cost();

}  // namespace unittest
}  // namespace crocoddyl

#endif  // CROCODDYL_COST_FACTORY_HPP_

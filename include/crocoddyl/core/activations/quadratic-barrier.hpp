///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2018-2020, LAAS-CNRS, University of Edinburgh
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#ifndef CROCODDYL_CORE_ACTIVATIONS_QUADRATIC_BARRIER_HPP_
#define CROCODDYL_CORE_ACTIVATIONS_QUADRATIC_BARRIER_HPP_

#include <stdexcept>
#include <math.h>

#include "crocoddyl/core/fwd.hpp"
#include "crocoddyl/core/utils/exception.hpp"
#include "crocoddyl/core/activation-base.hpp"

namespace crocoddyl {

template <typename _Scalar>
struct ActivationBoundsTpl {
  typedef _Scalar Scalar;
  typedef MathBaseTpl<Scalar> MathBase;
  typedef typename MathBase::VectorXs VectorXs;
  typedef typename MathBase::MatrixXs MatrixXs;

  ActivationBoundsTpl(const VectorXs& lower, const VectorXs& upper, const Scalar& b = (Scalar)1.)
      : lb(lower), ub(upper), beta(b) {
    if (lb.size() != ub.size()) {
      throw_pretty("Invalid argument: "
                   << "The lower and upper bounds don't have the same dimension (lb,ub dimensions equal to " +
                          std::to_string(lb.size()) + "," + std::to_string(ub.size()) + ", respectively)");
    }
    if (beta < 0 || beta > 1.) {
      throw_pretty("Invalid argument: "
                   << "The range of beta is between 0 and 1");
    }
    for (std::size_t i = 0; i < static_cast<std::size_t>(lb.size()); ++i) {
      if (std::isfinite(lb(i)) && std::isfinite(ub(i))) {
        if (lb(i) - ub(i) > 0) {
          throw_pretty("Invalid argument: "
                       << "The lower and upper bounds are badly defined; ub has to be bigger / equals to lb");
        }
      }
    }

    if (beta >= 0 && beta <= 1.) {
      VectorXs m = (Scalar)0.5 * (lower + upper);
      VectorXs d = (Scalar)0.5 * (upper - lower);
      lb = m - beta * d;
      ub = m + beta * d;
    } else {
      beta = 1.;
    }
  }
  ActivationBoundsTpl(const ActivationBoundsTpl& bounds) : lb(bounds.lb), ub(bounds.ub), beta(bounds.beta) {}
  ActivationBoundsTpl() : beta(1.) {}

  VectorXs lb;
  VectorXs ub;
  Scalar beta;
};

template <typename _Scalar>
class ActivationModelQuadraticBarrierTpl : public ActivationModelAbstractTpl<_Scalar> {
 public:
  typedef _Scalar Scalar;
  typedef MathBaseTpl<Scalar> MathBase;
  typedef ActivationModelAbstractTpl<Scalar> Base;
  typedef ActivationDataAbstractTpl<Scalar> ActivationDataAbstract;
  typedef ActivationDataQuadraticBarrierTpl<Scalar> ActivationDataQuadraticBarrier;
  typedef ActivationBoundsTpl<Scalar> ActivationBounds;
  typedef typename MathBase::VectorXs VectorXs;
  typedef typename MathBase::MatrixXs MatrixXs;

  explicit ActivationModelQuadraticBarrierTpl(const ActivationBounds& bounds)
      : Base(bounds.lb.size()), bounds_(bounds){};
  ~ActivationModelQuadraticBarrierTpl(){};

  virtual void calc(const boost::shared_ptr<ActivationDataAbstract>& data, const Eigen::Ref<const VectorXs>& r) {
    if (static_cast<std::size_t>(r.size()) != nr_) {
      throw_pretty("Invalid argument: "
                   << "r has wrong dimension (it should be " + std::to_string(nr_) + ")");
    }

    boost::shared_ptr<ActivationDataQuadraticBarrier> d =
        boost::static_pointer_cast<ActivationDataQuadraticBarrier>(data);

    d->rlb_min_ = (r - bounds_.lb).array().min(0.);
    d->rub_max_ = (r - bounds_.ub).array().max(0.);
    data->a_value =
        Scalar(0.5) * d->rlb_min_.matrix().squaredNorm() + Scalar(0.5) * d->rub_max_.matrix().squaredNorm();
  };

  virtual void calcDiff(const boost::shared_ptr<ActivationDataAbstract>& data, const Eigen::Ref<const VectorXs>& r) {
    if (static_cast<std::size_t>(r.size()) != nr_) {
      throw_pretty("Invalid argument: "
                   << "r has wrong dimension (it should be " + std::to_string(nr_) + ")");
    }

    boost::shared_ptr<ActivationDataQuadraticBarrier> d =
        boost::static_pointer_cast<ActivationDataQuadraticBarrier>(data);
    data->Ar = (d->rlb_min_ + d->rub_max_).matrix();
    data->Arr.diagonal() =
        (((r - bounds_.lb).array() <= 0.) + ((r - bounds_.ub).array() >= 0.)).matrix().template cast<Scalar>();
  };

  virtual boost::shared_ptr<ActivationDataAbstract> createData() {
    return boost::make_shared<ActivationDataQuadraticBarrier>(this);
  };

  const ActivationBounds& get_bounds() const { return bounds_; };
  void set_bounds(const ActivationBounds& bounds) { bounds_ = bounds; };

 protected:
  using Base::nr_;

 private:
  ActivationBounds bounds_;
};

template <typename _Scalar>
struct ActivationDataQuadraticBarrierTpl : public ActivationDataAbstractTpl<_Scalar> {
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  typedef _Scalar Scalar;
  typedef MathBaseTpl<Scalar> MathBase;
  typedef typename MathBase::ArrayXs ArrayXs;
  typedef ActivationDataAbstractTpl<Scalar> Base;

  template <typename Activation>
  explicit ActivationDataQuadraticBarrierTpl(Activation* const activation)
      : Base(activation), rlb_min_(activation->get_nr()), rub_max_(activation->get_nr()) {
    rlb_min_.setZero();
    rub_max_.setZero();
  }

  ArrayXs rlb_min_;
  ArrayXs rub_max_;
  using Base::a_value;
  using Base::Ar;
  using Base::Arr;
};

}  // namespace crocoddyl

#endif  // CROCODDYL_CORE_ACTIVATIONS_QUADRATIC_BARRIER_HPP_

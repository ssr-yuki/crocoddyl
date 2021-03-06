///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2018-2019, LAAS-CNRS
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "crocoddyl/core/utils/exception.hpp"
#include "crocoddyl/multibody/impulse-base.hpp"

namespace crocoddyl {

template <typename Scalar>
ImpulseModelAbstractTpl<Scalar>::ImpulseModelAbstractTpl(boost::shared_ptr<StateMultibody> state,
                                                         const std::size_t& ni)
    : state_(state), ni_(ni) {}

template <typename Scalar>
ImpulseModelAbstractTpl<Scalar>::~ImpulseModelAbstractTpl() {}

template <typename Scalar>
void ImpulseModelAbstractTpl<Scalar>::updateForceDiff(const boost::shared_ptr<ImpulseDataAbstract>& data,
                                                      const MatrixXs& df_dq) const {
  assert_pretty(
      (static_cast<std::size_t>(df_dq.rows()) == ni_ || static_cast<std::size_t>(df_dq.cols()) == state_->get_nv()),
      "df_dq has wrong dimension");
  data->df_dq = df_dq;
}

template <typename Scalar>
boost::shared_ptr<ImpulseDataAbstractTpl<Scalar> > ImpulseModelAbstractTpl<Scalar>::createData(
    pinocchio::DataTpl<Scalar>* const data) {
  return boost::make_shared<ImpulseDataAbstract>(this, data);
}

template <typename Scalar>
const boost::shared_ptr<StateMultibodyTpl<Scalar> >& ImpulseModelAbstractTpl<Scalar>::get_state() const {
  return state_;
}

template <typename Scalar>
const std::size_t& ImpulseModelAbstractTpl<Scalar>::get_ni() const {
  return ni_;
}

}  // namespace crocoddyl

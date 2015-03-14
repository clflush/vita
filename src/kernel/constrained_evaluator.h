/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2014-2015 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_CONSTRAINED_EVALUATOR_H)
#define      VITA_CONSTRAINED_EVALUATOR_H

#include "kernel/evaluator.h"

namespace vita
{
template<class T> using penalty_func_t = std::function<double (const T &)>;

///
/// \tparam T the type of individual used
/// \tparam E the basic evaluator
/// \tparam P the penalty function
///
/// The class merges a basic evaluator and a penalty function into a new
/// combined evaluator.
///
/// The new evaluator calculates a augmented fitness of this form:
///     (-penalty, base fitness)
///
template<class T, class E, class P>
class constrained_evaluator : public evaluator<T>
{
public:
  constrained_evaluator(E, P);

  virtual fitness_t operator()(const T &) override;
  virtual fitness_t fast(const T &) override;

  virtual double accuracy(const T &) const override;

  virtual std::unique_ptr<lambda_f<T>> lambdify(const T &) const override;

private:
  // Base evaluator.
  E eva_;

  // Penalty function.
  P penalty_;
};

#include "kernel/constrained_evaluator.tcc"
}  // namespace vita

#endif  // include guard

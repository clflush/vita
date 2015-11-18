/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2015 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_INTERPRETER_H)
#define      VITA_INTERPRETER_H

#include "kernel/core_interpreter.h"
#include "kernel/function.h"
#include "kernel/gene.h"
#include "kernel/matrix.h"
#include "kernel/vitafwd.h"

namespace vita
{
///
/// \brief A template specialization of the core_interpreter class
///
/// \tparam T the type of individual used
///
/// \note
/// This class would like to be a generic `interpreter<T>` implementation,
/// but interpreter and individual are strongly coupled objects: the
/// interpreter must be build around the peculiarities of the specific
/// individual class. So don't expect to use this template for `T` different
/// from `i_mep` without some modifications.
///
/// \note
/// This is an example of dependency injection via constructor injection: the
/// class gets the dependencies as an argument to its constructor. It saves
/// the reference to dependent classes as a private variables.
/// All the dependencies are truly visible with constructor signature and
/// cyclic dependencies don't happen because of the well-defined order of
/// instantiation.
///
/// \see
/// - <http://en.wikipedia.org/wiki/Dependency_injection>
/// - <http://stackoverflow.com/q/4542789/3235496>
/// - <http://stackoverflow.com/q/12387239/3235496>
/// - <http://stackoverflow.com/q/1974682/3235496>
///
template<class T>
class interpreter : public core_interpreter
{
public:
  explicit interpreter(const T *, interpreter * = nullptr);

  any fetch_param();
  any fetch_arg(unsigned);
  any fetch_adf_arg(unsigned);
  index_t fetch_index(unsigned) const;

  const T &program() const { return *prg_; }

private:  // Private support methods
  any run_locus(const locus &);
  double penalty_locus(const locus &);

  // Non-virtual interface
  virtual any run_nvi() override;
  virtual double penalty_nvi() override;
  virtual bool debug_nvi() const override;

private:  // Private data members
  const T *prg_;

  struct elem_ {bool valid; any value;};
  mutable matrix<elem_> cache_;

  // Instruction pointer.
  locus ip_;

  // This is a pointer since we need to describe a one-or-zero relationship.
  interpreter *context_;
};

#include "kernel/interpreter.tcc"

/// \example example5.cc
/// Output value calculation for an individual.
}  // namespace vita

#endif  // Include guard

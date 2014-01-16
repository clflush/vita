/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2014 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(INTERPRETER_H)
#define      INTERPRETER_H

#include <boost/optional.hpp>

#include "kernel/matrix.h"

namespace vita
{
  class individual;

  ///
  /// \brief Minimum interface of an interpreter
  ///
  /// \tparam T the type of individual used.
  ///
  template<class T>
  class core_interpreter
  {
  public:
    explicit core_interpreter(const T &, core_interpreter<T> * = nullptr);

    virtual any run() = 0;

    virtual bool debug() const;

  protected:
    const T &prg_;

    core_interpreter<T> *const context_;
  };

  ///
  /// \brief "Run" the individual
  ///
  /// \tparam T the type of individual used.
  ///
  /// The interpreter class "executes" an individual (a program) in its
  /// environment.
  ///
  template<class T> class interpreter;

  ///
  /// \brief A template specialization for interpreter<T> class
  ///
  /// \note
  /// We don't have a generic interpreter<T> implementation (e.g. see the
  /// lambda_f source code) because interpreter and individual are strongly
  /// coupled: the interpreter must be build around the peculiarities of
  /// the specific individual class.
  ///
  template<>
  class interpreter<individual> : public core_interpreter<individual>
  {
  public:
    explicit interpreter(const individual &,
                         interpreter<individual> * = nullptr);

    virtual any run() override;

    any fetch_param();
    any fetch_arg(unsigned);
    any fetch_adf_arg(unsigned);

    virtual bool debug() const override;

  private:  // Private methods.
    any run_locus(const locus &);

  private:
    // Instruction pointer.
    locus ip_;

    mutable matrix<boost::optional<any>> cache_;
  };

  ///
  /// \example example5.cc
  /// Output value calculation for an individual.
  ///

#include "kernel/interpreter_inl.h"
}  // namespace vita

#endif  // INTERPRETER_H

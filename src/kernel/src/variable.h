/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2012-2020 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_SRC_VARIABLE_H)
#define      VITA_SRC_VARIABLE_H

#include "kernel/src/interpreter.h"
#include "kernel/terminal.h"

namespace vita
{
///
/// Represents an input argument (feature) for a symbolic regression or
/// classification problem.
///
class variable : public terminal
{
public:
  variable(const std::string &name, unsigned var_id, category_t t = 0)
    : terminal(name, t), var_(var_id)
  {}

  bool input() const override { return true; }

  /// \return the name of the variable
  std::string display(terminal::param_t, format) const final
  { return name(); }

  /// \return the value of the variable
  ///
  /// \note Requires a src_interpreter to work.
  value_t eval(core_interpreter *i) const override
  {
    return static_cast<src_interpreter<i_mep> *>(i)->fetch_var(var_);
  }

private:
  unsigned var_;
};

}  // namespace vita

#endif  // include guard

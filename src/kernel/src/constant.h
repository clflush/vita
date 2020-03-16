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

#if !defined(VITA_SRC_CONSTANT_H)
#define      VITA_SRC_CONSTANT_H

#include "kernel/terminal.h"
#include "utility/utility.h"

namespace vita
{

///
/// A constant value (`bool`, `int`, `double`, `string`).
///
template<class T>
class constant : public terminal
{
public:
  explicit constant(const std::string &c, category_t t = 0)
    : terminal(c, t), val_(lexical_cast<T>(c)) {}
  explicit constant(T c, category_t t = 0)
    : constant(std::to_string(c), t) {}

  /// \return the string representation of the constant
  ///
  /// \remark
  /// Arguments aren't used: the value of a constant is stored within the
  /// object.
  std::string display(terminal::param_t, format) const final
  { return std::to_string(static_cast<T>(val_)); }

  /// \return the value of the constant
  ///
  /// \remark
  /// The argument is not used: the value of a constant is stored within the
  /// object and we don't need an interpreter to discover it.
  value_t eval(core_interpreter *) const override { return val_; }

private:
  T val_;
};

template<>
class constant<std::string> : public terminal
{
public:
  explicit constant(const std::string &c, category_t t = 0)
    : terminal("\"" + c + "\"", t), val_(c) {}
  explicit constant(const char c[], category_t t = 0)
    : constant(std::string(c), t) {}

  std::string display(param_t, format) const final
  {
    return "\"" + val_ + "\"";
  }

  ///
  /// \return the value of the constant
  ///
  /// \remark
  /// The argument is not used: the value of a constant is stored within the
  /// object and we don't need an interpreter to discover it.
  ///
  value_t eval(core_interpreter *) const override { return val_; }

private:
  std::string val_;
};

}  // namespace vita

#endif  // include guard

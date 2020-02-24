/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2020 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_BOOL_PRIMITIVE_H)
#define      VITA_BOOL_PRIMITIVE_H

#include <cstdlib>

#include "kernel/function.h"
#include "kernel/interpreter.h"
#include "kernel/terminal.h"

namespace vita::boolean
{

class zero : public terminal
{
public:
  explicit zero(const cvect &c) : terminal("0", c[0])
  { Expects(c.size() == 1); }

  std::any eval(core_interpreter *) const final { return false; }

  std::string display(terminal::param_t, format f) const final
  {
    switch (f)
    {
    case cpp_format:     return "false";
    case python_format:  return "False";
    default:             return     "0";
    }
  }
};

class one : public terminal
{
public:
  explicit one(const cvect &c) : terminal("1", c[0])
  { Expects(c.size() == 1); }

  std::any eval(core_interpreter *) const final { return true; }

  std::string display(terminal::param_t, format f) const final
  {
    switch (f)
    {
    case cpp_format:     return "true";
    case python_format:  return "True";
    default:             return    "1";
    }
  }
};

class l_and : public function
{
public:
  explicit l_and(const cvect &c) : function("AND", c[0], {c[0], c[0]})
  { Expects(c.size() == 1); }

  bool associative() const final { return true; }

  std::any eval(core_interpreter *ci) const final
  {
    auto &i(*static_cast<interpreter<i_mep> *>(ci));
    return std::any_cast<bool>(i.fetch_arg(0))
           && std::any_cast<bool>(i.fetch_arg(1));
  }

  std::string display(format f) const final
  {
    switch (f)
    {
    case python_format:  return "(%%1%% and %%2%%)";
    default:             return  "(%%1%% && %%2%%)";
    }
  }
};

class l_not : public function
{
public:
  explicit l_not(const cvect &c) : function("NOT", c[0], {c[0]})
  { Expects(c.size() == 1); }

  std::any eval(core_interpreter *ci) const final
  {
    auto &i(*static_cast<interpreter<i_mep> *>(ci));
    return !std::any_cast<bool>(i.fetch_arg(0));
  }

  std::string display(format f) const final
  {
    switch (f)
    {
    case python_format:  return "not(%%1%%)";
    default:             return   "!%%1%%";
    }
  }
};

class l_or : public function
{
public:
  explicit l_or(const cvect &c) : function("OR", c[0], {c[0], c[0]})
  { Expects(c.size() == 1); }

  bool associative() const final { return true; }

  std::any eval(core_interpreter *ci) const final
  {
    auto i(static_cast<interpreter<i_mep> *>(ci));
    return std::any_cast<bool>(i->fetch_arg(0))
           || std::any_cast<bool>(i->fetch_arg(1));
  }

  std::string display(format f) const final
  {
    switch (f)
    {
    case python_format:  return "(%%1%% or %%2%%)";
    default:             return "(%%1%% || %%2%%)";
    }
  }
};

}  // namespace vita::boolean

#endif  // include guard

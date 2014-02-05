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

#include <boost/lexical_cast.hpp>

#include "kernel/adf.h"
#include "kernel/interpreter.h"

namespace vita
{
  ///
  /// \param[in] ind individual whose code is used as ADF.
  ///
  adf_core::adf_core(const individual &ind) : code_(ind), id_(adf_count())
  {
  }

  ///
  /// \param[in] prefix a string identifying adf type.
  /// \return a string identifying an ADF/ADT.
  ///
  std::string adf_core::display(const std::string &prefix) const
  {
    return prefix + boost::lexical_cast<std::string>(id_);
  }

  ///
  /// \return the code (program) of the ADF.
  ///
  const individual &adf_core::get_code() const
  {
    return code_;
  }

  ///
  /// \return \c true if the \a object passes the internal consistency check.
  ///
  bool adf_core::debug() const
  {
    return code_.eff_size() >= 2;
  }

  ///
  /// \param[in] ind the code for the ADF.
  /// \param[in] sv categories of the function arguments.
  /// \param[in] w the weight of the ADF.
  ///
  adf::adf(const individual &ind, const std::vector<category_t> &sv,
           unsigned w)
    : function("ADF", ind.category(), sv, w), adf_core(ind)
  {
    assert(ind.debug() && ind.eff_size() >= 2);

    assert(debug());
  }

  ///
  /// \param[in] i the context in which this ADF is evaluated.
  /// \return the output of the ADF.
  ///
  /// Adf functions need input parameters from the a context (contrary to
  /// adt::eval).
  ///
  any adf::eval(interpreter<individual> *i) const
  {
    return interpreter<individual>(code_, i).run();
  }

  ///
  /// \return the name of the ADF.
  ///
  std::string adf::display() const
  {
    return adf_core::display(symbol::display());
  }

  ///
  /// \return \c true.
  ///
  bool adf::auto_defined() const
  {
    return true;
  }

  ///
  /// \return \c true if the \a object passes the internal consistency check.
  ///
  bool adf::debug() const
  {
    // No recursive calls.
    for (const auto &l : code_)
      if (code_[l].sym == this)
        return false;

    if (!adf_core::debug())
      return false;

    return function::debug();
  }

  ///
  /// \param[in] ind the code for the ADT.
  /// \param[in] w the weight of the ADT.
  ///
  adt::adt(const individual &ind, unsigned w)
    : terminal("ADT", ind.category(), false, false, w), adf_core(ind)
  {
    assert(ind.debug());
    assert(ind.eff_size() >= 2);

    assert(debug());
  }

  ///
  /// \return the output of the ADT.
  ///
  /// Adt hasn't input parameters so the context is ignored (contrary to
  /// adf::eval).
  ///
  any adt::eval(interpreter<individual> *) const
  {
    return interpreter<individual>(code_).run();
  }

  ///
  /// \return the name of the ADT.
  ///
  std::string adt::display() const
  {
    return adf_core::display(symbol::display());
  }

  ///
  /// \return \c true.
  ///
  bool adt::auto_defined() const
  {
    return true;
  }

  ///
  /// \return \c true if the \a object passes the internal consistency check.
  ///
  bool adt::debug() const
  {
    // No recursive calls.
    for (const auto &l : code_)
      if (code_[l].sym == this)
        return false;

    if (!adf_core::debug())
      return false;

    return terminal::debug();
  }
}  // namespace vita

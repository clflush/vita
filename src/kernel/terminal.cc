/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2017 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#include "kernel/terminal.h"

namespace vita
{
///
/// \param[in] v additional informations regarding parametric symbols
/// \param[in] f output format
/// \return      a string representing the symbol in the `f` format
///
/// The `v` argument is required for parametric symbols (i.e. for a numeric
/// terminal we have to print 123 rather than "NUMBER").
///
std::string terminal::display(double v, format) const
{
  return name() + "_" + std::to_string(v);
}

///
/// \return `true` if the function passes the internal consistency check
///
bool terminal::debug() const
{
  return symbol::debug();
}

}  // namespace vita

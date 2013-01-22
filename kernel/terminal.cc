/**
 *
 *  \file terminal.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011, 2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "terminal.h"

namespace vita
{
  /// This is the default value for the terminal constructor. Weights are
  /// used by the symbol_set::roulette method to control the probability of
  /// extraction of the symbols.
  unsigned terminal::default_weight(100);

  ///
  /// \param[in] dis string printed to identify the terminal.
  /// \param[in] c category of the terminal.
  /// \param[in] par true if the terminal is parametric.
  /// \param[in] in true if the terminal is an input value.
  /// \param[in] w weight used for symbol frequency control.
  ///
  terminal::terminal(const std::string &dis, category_t c, bool in, bool par,
                     unsigned w)
    : symbol(dis, c, w), parametric_(par), input_(in)
  {
    assert(check());
  }

  ///
  /// \return \c true if terminal was loaded correctly.
  ///
  bool terminal::load(std::istream &in)
  {
    const bool ok(symbol::load(in));

    in >> parametric_;
    in >> input_;

    return ok && in.good();
  }

  ///
  /// \return \c true if terminal was saved correctly.
  ///
  bool terminal::save(std::ostream &out) const
  {
    const bool ok(symbol::save(out));

    out << parametric_ << ' ' << input_ << std::endl;

    return ok && out.good();
  }

  ///
  /// \return \c true if the object passes the internal consistency check.
  ///
  bool terminal::check() const
  {
    return symbol::check();
  }
}  // Namespace vita

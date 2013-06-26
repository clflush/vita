/**
 *
 *  \file random.h
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "kernel/random.h"

namespace vita
{
  ///
  /// \param[in] s the seed for the random number generator.
  ///
  /// The seed is used to initalize the random number generator. With the same
  /// seed the numbers produced will be the same every time the program is
  /// run.
  ///
  /// \note
  /// One common method to change this is to seed with the current time
  /// (\c std::time(0)) but the preferred way in Vita is the random::randomize
  /// method (based on \c std::random_device).
  ///
  void random::seed(unsigned s)
  {
    engine().seed(s);
  }

  ///
  /// Sets the shared engine to an unpredictable state.
  ///
  void random::randomize()
  {
    static std::random_device rd{};
    seed(rd());
  }

  ///
  /// \param[in] base
  /// \param[in] width
  /// \param[in] n
  /// \return the index of the choosen individual.
  ///
  /// Return a random number in the range [base-width/2, base+width/2] mod n.
  /// This is like a circular protractor marked from 0 to \c n: starting from
  /// position \a base we want a random number whose distance from \a base
  /// is less than or equal to \c width/2.
  /// If \c base is greater than \c n we take a random starting position on the
  /// protractor.
  ///
  unsigned random::ring(unsigned base, unsigned width, unsigned n)
  {
    assert(width);
    assert(n > 1);

    if (base >= n)
      return random::between<unsigned>(0, n);

    if (width > n)
      width = n;

    const auto offset(n + base - width / 2);

    return (offset + random::between<unsigned>(0, width)) % n;
  }
}  // Namespace vita

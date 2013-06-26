/**
 *
 *  \file vita.h
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 *
 *  \mainpage VITA v0.9.8
 *
 *  \section Introduction
 *  Welcome to the Vita project.
 *
 *  This is the reference guide for the Vita APIs.
 *
 *  \section note_sec Notes
 *  New versions of this program will be available at
 *  http://code.google.com/p/vita/
 *
 *  Please reports any suggestions and/or bugs to:
 *  <dl>
 *    <dt>Forum</dt>
 *    <dd>http://groups.google.com/group/vita-prj</dd>
 *    <dt>Issue tracking system</dt>
 *    <dd>http://code.google.com/p/vita/issues/list</dd>
 *  </dl>
 *
 */

#if !defined(VITA_H)
#define      VITA_H

#include <cassert>
#include <cstdint>
#include <memory>

#include <boost/spirit/home/support/detail/hold_any.hpp>

#include "kernel/compatibility_patch.h"

namespace vita
{
#if defined(VITA_NO_LIB)
#  define VITA_INLINE inline
#else
#  define VITA_INLINE
#endif

  /// This is the type used as key for symbol identification.
  typedef unsigned opcode_t;

  /// In an environmnet where a symbol such as + may have many different
  /// meanings, it is useful to specify a "domain of computation" to restrict
  /// attention to specific meanings of interest (e.g. double domain: 1 + 1 = 2;
  /// string domain: "a" + "b" = "ab").
  /// The operations of a domain are defined in files named after the domain
  /// and grouped in the \c primitive/ folder.
  enum domain_t {d_void = 0, d_bool, d_double, d_int, d_string};

  /// A category provide operations which supplement or supersede those of the
  /// domain but which are restricted to values lying in the (sub)domain by
  /// which is parametrized.
  /// For instance the number 4.0 (in the domain \c d_double) may be present in
  /// two distinct category: 2 (e.g. the category "km/h") and 3 (e.g. the
  /// category "kg").
  /// Categories are the way strong typing GP is implemented in Vita.
  typedef size_t category_t;

  /// Index in the genome. Locus is uniquely identified by a couple of indexes.
  typedef size_t index_t;

  ///
  /// When absolute value of two real numbers is under \a float_epsilon, they
  /// are considered equivalent.
  ///
  const double float_epsilon(0.0001);

  ///
  /// A shortcut for the any type (usually boost::any or boost::spirit::any).
  ///
  typedef boost::spirit::hold_any any;

  ///
  /// \tparam T a C++ type.
  /// \return the maximum value of type \a T.
  ///
  template<class T> inline T type_max(T)
  {
    return std::numeric_limits<T>::max();
  }

  ///
  /// \param a an any.
  /// \return the value contained in \a a.
  ///
  template<class T> inline T any_cast(const any &a)
  {
    // We must choose the right any_cast (it depends on any typedef).
    // The alternative is: return boost::any_cast<T>(a);
    return boost::spirit::any_cast<T>(a);
  }

  ///
  /// An implementation of make_unique() as proposed by Herb Sutter in
  /// GotW #102.
  ///
  template<typename T, typename ...Args>
  std::unique_ptr<T> make_unique(Args&& ...args)
  {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }

  /// \brief Prefix for debug messages.
  const char k_s_debug[] = "[DEBUG]";
  /// \brief Prefix for error messages.
  const char k_s_error[] = "[ERROR]";
  /// \brief Prefix for information messages.
  /// Information messages regard the status of the program.
  const char k_s_info[] = "[INFO]";
  /// \brief Prefix for warning messages.
  const char k_s_warning[] = "[WARNING]";

}  // namespace vita

/// \page page1 VITA Architecture
/// http://code.google.com/p/vita/wiki/Anatomy
/// \page page2 Contributor guidelines
/// http://code.google.com/p/vita/wiki/ContributorGuidelines

#endif  // VITA_H

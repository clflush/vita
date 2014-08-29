/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2014 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_INDIVIDUAL_NUM_GA_ITERATOR_H)
#define      VITA_INDIVIDUAL_NUM_GA_ITERATOR_H

///
/// \brief Iterator to scan the active genes of an \c individual
///
class i_num_ga::const_iterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t ;
  using value_type = locus;
  using pointer = const value_type *;
  using reference = const value_type &;

  ///
  /// \brief Builds an empty iterator.
  ///
  /// Empty iterator is used as sentry (it is the value returned by
  /// i_num_ga::end()).
  ///
  const_iterator() : sup_(0), i_(std::numeric_limits<category_t>::max()) {}

  ///
  /// \param[in] id an individual.
  ///
  explicit const_iterator(const i_num_ga &id) : sup_(id.parameters()), i_(0)
  {
  }

  ///
  /// \return locus of the next active symbol.
  ///
  const_iterator &operator++()
  {
    ++i_;

    if (i_ >= sup_)
      i_ = std::numeric_limits<category_t>::max();

    return *this;;
  }

  ///
  /// \param[in] rhs second term of comparison.
  ///
  /// Returns \c true if iterators point to the same locus.
  ///
  bool operator==(const const_iterator &rhs) const
  {
    return i_ == rhs.i_;
  }

  bool operator!=(const const_iterator &rhs) const
  {
    return !(*this == rhs);
  }

  ///
  /// \return the current \a locus of the \a individual.
  ///
  value_type operator*() const
  {
    return {0, i_};
  }

private:  // Private data members
  const category_t sup_;
  category_t i_;
};  // class i_num_ga::const_iterator

#endif  // Include guard
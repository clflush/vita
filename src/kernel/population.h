/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2015 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_POPULATION_H)
#define      VITA_POPULATION_H

#include <fstream>

#include "kernel/environment.h"
#include "kernel/random.h"

namespace vita
{
///
/// \brief A group of individuals which may interact together (for example by
///        mating) producing offspring
///
/// \tparam T the type of the an individual
///
/// Typical population size in GP ranges from ten to many thousands. The
/// population is organized in one or more layers that can interact in
/// many ways (depending on the evolution strategy).
///
template<class T>
class population
{
public:
  /// \brief Holds the coordinates of an individual in a population
  struct coord
  {
    unsigned layer;
    unsigned index;

    bool operator==(coord rhs) const
    { return layer == rhs.layer && index == rhs.index; }
    bool operator!=(coord rhs) const { return !(*this == rhs); }
  };

public:
  explicit population(const environment &);

  T &operator[](coord);
  const T &operator[](coord) const;

  unsigned allowed(unsigned) const;
  unsigned individuals() const;
  unsigned individuals(unsigned) const;

  void init_layer(unsigned);
  void add_layer();
  unsigned layers() const;
  void inc_age();
  void add_to_layer(unsigned, const T &);
  void pop_from_layer(unsigned);
  void set_allowed(unsigned, unsigned);

  const environment &env() const;

  bool debug(bool) const;

  // Iterators
  using layer_t = std::vector<T>;
  using const_iterator = typename std::vector<layer_t>::const_iterator;

  const_iterator begin() const;
  const_iterator end() const;

  // Serialization
  bool load(std::istream &, const environment &);
  bool save(std::ostream &) const;

private:  // Private data members
  const environment *env_;

  std::vector<layer_t> pop_;
  std::vector<unsigned> allowed_;
};

#include "kernel/population.tcc"

///
/// \example example2.cc
/// Creates a random population and shows its content.
///
}  // namespace vita

#endif  // Include guard

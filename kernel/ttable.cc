/**
 *
 *  \file ttable.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "kernel/ttable.h"
#include "kernel/individual.h"

namespace vita
{
  ///
  /// \param[in] in input stream.
  /// \return \c true if hash_t loaded correctly.
  ///
  /// \note
  /// If the load operation isn't successful the current hash_t isn't changed.
  ///
  bool hash_t::load(std::istream &in)
  {
    hash_t tmp;

    if (!(in >> tmp.data[0] >> tmp.data[1]))
    {
      std::cout << tmp.data[0] << ' ' << tmp.data[1] << std::endl;
      return false;
    }

    *this = tmp;

    return true;
  }

  ///
  /// \param[out] out output stream.
  /// \return \c true if hash_t was saved correctly.
  ///
  bool hash_t::save(std::ostream &out) const
  {
    out << data[0] << ' ' << data[1] << std::endl;

    return out.good();
  }

  ///
  /// \param[in] bits 2^\a bits is the number of elements of the table.
  ///
  /// Creates a new transposition (hash) table.
  ///
  ttable::ttable(unsigned bits)
    : k_mask((1 << bits) - 1), table_(new slot[1 << bits]), seal_(1),
      probes_(0), hits_(0)
  {
    assert(debug());
  }

  ///
  /// Standard destructor.
  ///
  ttable::~ttable()
  {
    delete [] table_;
  }

  ///
  /// \param[in] u the signature of an individual.
  /// \return an index in the hash table.
  ///
  inline
  size_t ttable::index(const hash_t &h) const
  {
    return h.data[0] & k_mask;
  }

  ///
  /// Clears the content and the statistical informations of the table
  /// (allocated size isn't changed).
  ///
  void ttable::clear()
  {
    probes_ = hits_ = 0;

    ++seal_;

    //for (size_t i(0); i <= k_mask; ++i)
    //{
    //  table_[i].hash = hash_t();
    //  table_[i].fitness = {};
    //}
  }

  ///
  /// \param[in] ind individual whose informations we have to clear.
  ///
  /// Clears the cached information for individual \a ind.
  ///
  void ttable::clear(const individual &ind)
  {
    const hash_t h(ind.signature());

    table_[index(h)].hash = hash_t();

    // An alternative to invalidate the slot:
    //   table_[index(h)].seal = 0;
    // It works because the first valid seal is 1.
  }

  ///
  /// Resets the \a seen counter.
  ///
#if defined(CLONE_SCALING)
  void ttable::reset_seen()
  {
    probes_ = hits_ = 0;

    for (size_t i(0); i <= k_mask; ++i)
      table_[i].seen = 0;
  }
#endif

  ///
  /// \brief Looks for the fitness of an individual in the transposition table.
  ///
  /// \param[in] ind the individual to look for.
  /// \param[out] fitness the fitness of the individual (if present).
  /// \return \c true if \a ind is found in the transposition table, \c false
  ///         otherwise.
  ///
  bool ttable::find(const individual &ind, fitness_t *const fitness) const
  {
    ++probes_;

    const hash_t h(ind.signature());

    const slot &s(table_[index(h)]);

    const bool ret(seal_ == s.seal && h == s.hash);

    if (ret)
    {
#if defined(CLONE_SCALING)
      ++s.seen;
#endif
      ++hits_;
      *fitness = s.fitness;
    }

    return ret;
  }

  ///
  /// \param[in] ind the individual to look for.
  /// \return number of times \a ind has been looked for.
  ///
  unsigned ttable::seen(const individual &ind) const
  {
    const hash_t h(ind.signature());
    const slot &s(table_[index(h)]);

    const bool ret(seal_ == s.seal && h == s.hash);

#if defined(CLONE_SCALING)
    return ret ? s.seen : 0;
#else
    return ret;
#endif
  }

  ///
  /// \param[in] ind a (possibly) new individual to be stored in the table.
  /// \param[in] fitness the fitness of the individual.
  ///
  /// Stores fitness information in the transposition table.
  ///
  void ttable::insert(const individual &ind, const fitness_t &fitness)
  {
    slot s;
    s.hash    = ind.signature();
    s.fitness =         fitness;
    s.seal    =           seal_;
#if defined(CLONE_SCALING)
    s.seen    =               1;
#endif

    table_[index(s.hash)] = s;
  }

  ///
  /// \param[in] in input stream.
  /// \return \c true if ttable loaded correctly.
  ///
  /// \note
  /// If the load operation isn't successful the current object isn't changed.
  ///
  bool ttable::load(std::istream &in)
  {
    decltype(seal_) t_seal;
    if (!(in >> t_seal))
      return false;

    decltype(probes_) t_probes;
    if (!(in >> t_probes))
      return false;

    decltype(hits_) t_hits;
    if (!(in >> t_hits))
      return false;

    size_t n;
    if (!(in >> n))
      return false;

    seal_ = t_seal;
    probes_ = t_probes;
    hits_ = t_hits;

    for (size_t i(0); i < n; ++i)
    {
      slot s;

      if (!s.hash.load(in))
        return false;
      if (!s.fitness.load(in))
        return false;
      if (!(in >> s.seal))
        return false;
#if defined(CLONE_SCALING)
      if (!(in >> s.seen))
        return false;
#endif

      table_[index(s.hash)] = s;
    }

    return true;
  }

  ///
  /// \param[out] out output stream.
  /// \return \c true if ttable was saved correctly.
  ///
  bool ttable::save(std::ostream &out) const
  {
    out << seal_ << ' ' << probes_ << ' ' << hits_ << std::endl;

    size_t num(0);
    for (size_t i(0); i <= k_mask; ++i)
      if (!table_[i].hash.empty())
        ++num;
    out << num << std::endl;

    for (size_t i(0); i <= k_mask; ++i)
      if (!table_[i].hash.empty())
      {
        const slot &s(table_[i]);
        s.hash.save(out);
        s.fitness.save(out);
        out << s.seal;
#if defined(CLONE_SCALING)
        out << ' ' << s.seen;
#endif
        out << std::endl;
      }

    return out.good();
  }

  ///
  /// \return \c true if the table passes the internal consistency check.
  ///
  bool ttable::debug() const
  {
    return probes() >= hits();
  }
}  // namespace vita

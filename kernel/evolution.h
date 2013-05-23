/**
 *
 *  \file evolution.h
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#if !defined(EVOLUTION_H)
#define      EVOLUTION_H

#include <algorithm>
#include <csignal>

#include "analyzer.h"
#include "evaluator_proxy.h"
#include "evolution_operation.h"
#include "evolution_replacement.h"
#include "evolution_selection.h"
#include "population.h"
#include "random.h"
#include "timer.h"

namespace vita
{
  template<class T>
  class summary
  {
  public:  // Constructor and support functions.
    summary();

    void clear();

  public:   // Serialization.
    bool load(std::istream &, const environment &);
    bool save(std::ostream &) const;

  public:  // Public data members.
    analyzer az;

    struct best_
    {
      T             ind;
      fitness_t fitness;
    };

    boost::optional<best_> best;

    double speed;

    std::uintmax_t crossovers, mutations;

    unsigned gen, last_imp;
  };

  ///
  /// Progressively evolves a population of programs over a series of
  /// generations.
  ///
  /// The evolutionary search uses the Darwinian principle of natural selection
  /// (survival of the fittest) and analogs of various naturally occurring
  /// operations, including crossover (sexual recombination), mutation...
  ///
  template<class T>
  class evolution
  {
  public:
    evolution(const environment &, evaluator *,
              std::function<bool (const summary<T> &)> = nullptr,
              std::function<void (unsigned)> = nullptr);

    const summary<T> &run(unsigned);

    const vita::population<T> &population() const;
    vita::population<T> &population();

    fitness_t fitness(const T &) const;
    fitness_t fast_fitness(const T &) const;
    unsigned seen(const T &i) const;

    bool debug(bool) const;

  public:  // Public data members.
    typename selection_strategy<T>::ptr     selection;
    typename operation_strategy<T>::ptr     operation;
    typename replacement_strategy<T>::ptr replacement;

  private:  // Private support methods.
    const environment &env() const { return pop_.env(); }
    double get_speed(double) const;
    analyzer get_stats() const;
    void log(unsigned) const;
    void print_progress(unsigned, unsigned, bool) const;
    bool stop_condition(const summary<T> &) const;

  private:  // Private data members.
    vita::population<T> pop_;
    evaluator          *eva_;
    summary<T>        stats_;

    std::function<bool (const summary<T> &)> external_stop_condition_;
    std::function<void (unsigned)> shake_data_;
  };

#include "evolution_inl.h"

  ///
  /// \example example6.cc
  /// Simple symbolic regression example (\f$x^2+y^2-z^2\f$).
  ///
}  // namespace vita

#endif  // EVOLUTION_H

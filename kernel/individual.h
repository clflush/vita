/**
 *
 *  \file individual.h
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#if !defined(INDIVIDUAL_H)
#define      INDIVIDUAL_H

#include <cmath>
#include <functional>
#include <iomanip>
#include <set>

#include "kernel/environment.h"
#include "kernel/gene.h"
#include "kernel/locus.h"
#include "kernel/matrix.h"
#include "kernel/symbol_set.h"
#include "kernel/ttable.h"

namespace vita
{
  ///
  /// A single member of a \a population. Each individual contains a genome
  /// which represents a possible solution to the task being tackled (i.e. a
  /// point in the search space).
  ///
  class individual
  {
  public:
    individual(const environment &, const symbol_set &);

    // Visualization/output methods.
    void dump(std::ostream &) const;
    void graphviz(std::ostream &, const std::string & = "") const;
    void in_line(std::ostream &) const;
    void list(std::ostream &) const;
    void tree(std::ostream &) const;

    unsigned mutation()
    { assert(env_->p_mutation); return mutation(*env_->p_mutation); }
    unsigned mutation(double);

    individual crossover(const individual &) const;

    std::vector<locus> blocks() const;
    individual destroy_block(index_t) const;
    individual get_block(const locus &) const;
    individual replace(const gene &) const;
    individual replace(const locus &, const gene &) const;
    individual replace(const std::vector<gene> &) const;

    individual generalize(size_t, std::vector<locus> *const) const;

    bool operator==(const individual &x) const;
    size_t distance(const individual &) const;

    hash_t signature() const;

    /// This is a measure of how long an individual's family of genotypic
    /// material has been in the population. Randomly generated individuals,
    /// such as those that are created when the search algorithm are started,
    /// start with an age of 0. Each generation that an individual stays in the
    /// population (such as through elitism) its age is increased by one.
    /// Individuals that are created through mutation or recombination take the
    /// age of their oldest parent.
    /// This differs from conventional measures of age, in which individuals
    /// created through applying some type of variation to an existing
    /// individual (e.g. mutation or recombination) start with an age of 0.
    unsigned age() const { return age_; }
    void inc_age() { ++age_; }

    const environment &env() const { return *env_; }
    const symbol_set &sset() const { return *sset_; }

    bool debug(bool = true) const;

    ///
    /// \param[in] l locus of a \c gene.
    /// \return the l-th \c gene of \a this \c individual.
    ///
    const gene &operator[](const locus &l) const { return genome_(l); }

    ///
    /// \return the total size of the individual (effective size + introns).
    ///
    /// The size is constant for any individual (it's choosen at initialization
    /// time).
    /// \see eff_size
    ///
    size_t size() const { return genome_.rows(); }

    size_t eff_size() const;

    category_t category() const;

    ///
    /// \param[in] l locus of a \c gene.
    /// \param[in] g a new gene.
    ///
    /// Set locus \a l of the genome to \a g. Please note that this is one of
    /// the very few methods that aren't const.
    ///
    void set(const locus &l, const gene &g)
    {
      genome_(l) = g;
      signature_.clear();
    }

    class const_iterator;
    friend class interpreter;

  public:   // Serialization.
    bool load(std::istream &);
    bool save(std::ostream &) const;

  private:  // Private support functions.
    template<class T = std::uint8_t> hash_t hash() const;
    template<class T> void pack(const locus &, std::vector<T> *const) const;
    void tree(std::ostream &, const locus &, unsigned, const locus &) const;

  private:  // Private data members.
    // This is the genome: the entire collection of genes (the entirety of an
    // organism's hereditary information).
    matrix<gene> genome_;

    // Note that sintactically distinct (but logically equivalent) individuals
    // have the same signature. This is a very interesting  property, useful
    // for individual comparison, information retrieval, entropy calculation...
    mutable hash_t signature_;

    // Starting point of the active code in this individual (the best sequence
    // of genes is starting here).
    locus best_;

    unsigned age_;

    const environment  *env_;
    const symbol_set  *sset_;
  };  // class individual


  std::ostream &operator<<(std::ostream &, const individual &);

  individual one_point_crossover(const individual &, const individual &);
  individual two_point_crossover(const individual &, const individual &);
  individual uniform_crossover(const individual &, const individual &);

  ///
  /// Iterator to scan the active genes of an \c individual.
  ///
  class individual::const_iterator
  {
  public:
    explicit const_iterator(const individual &);

    ///
    /// \return \c false when the iterator reaches the end.
    ///
    bool operator()() const
    { return l.index < ind_.size() && !loci_.empty(); }

    const locus operator++();

    ///
    /// \return reference to the current \a gene of the \a individual.
    ///
    const gene &operator*() const
    {
      assert(l.index < ind_.size());
      return ind_.genome_(l);
    }

    ///
    /// \return pointer to the current \c gene of the \c individual.
    ///
    const gene *operator->() const
    {
      assert(l.index < ind_.size());
      return &ind_.genome_(l);
    }

    locus l;

  private:
    const individual &ind_;
    std::set<locus>  loci_;
  };  // class individual::const_iterator

  ///
  /// \example example1.cc
  /// Creates a random individual and shows its content.
  ///
  /// \example example3.cc
  /// Performs three types of crossover between two random individuals.
  ///
}  // namespace vita

#endif  // INDIVIDUAL_H

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

#include "kernel/ga/i_num_ga.h"
#include "kernel/ttable_hash.h"

namespace vita
{
  ///
  /// \param[in] e base environment.
  /// \param[in] ss a symbol set.
  ///
  /// The process that generates the initial, random expressions has to be
  /// implemented so as to ensure that they do not violate the type system's
  /// constraints.
  ///
  i_num_ga::i_num_ga(const environment &e, const symbol_set &ss)
    : genome_(ss.categories()), signature_(), age_(0), env_(&e), sset_(&ss)
  {
    assert(e.debug(true, true));

    assert(size());

    const auto cs(ss.categories());
    assert(cs);

    for (auto c(decltype(cs){0}); c < cs; ++c)
      genome_[c] = gene(ss.roulette_terminal(c));

    assert(debug(true));
  }

  ///
  /// \param[out] s output stream.
  ///
  /// The output stream contains a graph, described in dot language
  /// (http://www.graphviz.org), of \c this individual.
  ///
  void i_num_ga::graphviz(std::ostream &s) const
  {
    s << "graph {";

    for (const auto &g : genome_)
      s << "g [label=" << g << ", shape=circle];";

    s << '}';
  }

  ///
  /// \param[out] s output stream
  ///
  /// Prints genes of the individual.
  ///
  std::ostream &i_num_ga::in_line(std::ostream &s) const
  {
    std::copy(genome_.begin(), genome_.end(), infix_iterator<gene>(s, " "));
    return s;
  }

  ///
  /// \param[out] s output stream
  ///
  /// Do you remember C=64 list? :-)
  ///
  /// 01 123.37
  /// 02 13
  /// 03 99.99
  /// ...
  ///
  std::ostream &i_num_ga::list(std::ostream &s) const
  {
    const auto cs(sset_->categories());
    const auto w(1 + static_cast<int>(std::log10(cs)));

    unsigned i(0);
    for (const auto &g : genome_)
    {
      assert(i == g.sym->category());

      s << '[' << std::setfill('0') << std::setw(w) << i++ << "] " << g
        << std::endl;
    }

    return s;
  }

  ///
  /// \param[out] s output stream.
  ///
  std::ostream &i_num_ga::tree(std::ostream &s) const
  {
    return in_line(s);
  }

  ///
  /// \param[in] p probability of gene mutation.
  /// \return number of mutations performed.
  ///
  /// A new individual is created mutating \c this.
  ///
  unsigned i_num_ga::mutation(double p)
  {
    assert(0.0 <= p && p <= 1.0);

    unsigned n(0);

    const auto cs(size());
    for (category_t c(0); c < cs; ++c)
      if (random::boolean(p))
      {
        ++n;

        genome_[c] = gene(sset_->roulette_terminal(c));
      }

    assert(debug());
    return n;
  }

  ///
  /// \brief Two points crossover
  /// \param[in] rhs the second parent.
  /// \return the result of the crossover (we only generate a single
  ///         offspring).
  ///
  /// We randomly select a parent (between \a this and \a p2) and a two loci
  /// (common crossover points). The offspring is created with genes from the
  /// choosen parent before the first crossover point and after the second
  /// crossover point; genes between crossover points are taken from the other
  /// parent.
  ///
  /// \note
  /// Parents must have the same size.
  ///
  i_num_ga i_num_ga::crossover(i_num_ga rhs) const
  {
    assert(rhs.debug());

    const auto cs(size());
    assert(cs == rhs.size());

    const auto cut1(random::sup(cs - 1));
    const auto cut2(random::between(cut1 + 1, cs));

    for (auto i(cut1); i < cut2; ++i)
      rhs.genome_[i] = genome_[i];

    rhs.age_ = std::max(age(), rhs.age());

    assert(rhs.debug());
    return rhs;
  }

  ///
  /// \return an iterator pointing to the first individual of the team.
  ///
  i_num_ga::const_iterator i_num_ga::begin() const
  {
    return genome_.begin();
  }

  ///
  /// \return an iterator pointing to a end-of-team sentry.
  ///
  i_num_ga::const_iterator i_num_ga::end() const
  {
    return genome_.end();
  }


  ///
  /// \return the signature of \c this individual.
  ///
  /// Identical individuals at genotypic level have the same signature
  ///
  hash_t i_num_ga::signature() const
  {
    if (signature_.empty())
      signature_ = hash();

    return signature_;
  }

  ///
  /// \return the signature of \c this individual.
  ///
  /// Converts \c this individual in a packed byte level representation and
  /// performs the MurmurHash3 algorithm on it.
  ///
  hash_t i_num_ga::hash() const
  {
    // From an individual to a packed byte stream...
    thread_local std::vector<unsigned char> packed;

    packed.clear();
    pack(&packed);

    /// ... and from a packed byte stream to a signature...
    const auto len(static_cast<unsigned>(
                     packed.size() * sizeof(packed[0])));  // Length in bytes

    return vita::hash(packed.data(), len, 1973);
  }

  ///
  /// \param[out] p byte stream compacted version of the gene sequence.
  ///
  void i_num_ga::pack(std::vector<unsigned char> *const p) const
  {
    for (const auto &g : genome_)
    {
      // Although 16 bit are enough to contain opcodes and parameters, they are
      // usually stored in unsigned variables (i.e. 32 or 64 bit) for
      // performance reasons.
      // Anyway before hashing opcodes/parameters we convert them to 16 bit
      // types to avoid hashing more than necessary.
      const auto opcode(static_cast<std::uint16_t>(g.sym->opcode()));
      assert(g.sym->opcode() <= std::numeric_limits<decltype(opcode)>::max());

      // DO NOT CHANGE reinterpret_cast type to std::uint8_t since even if
      // std::uint8_t has the exact same size and representation as
      // unsigned char, if the implementation made it a distinct, non-character
      // type, the aliasing rules would not apply to it
      // (see http://stackoverflow.com/q/16138237/3235496)
      const auto *const s1 = reinterpret_cast<const unsigned char *>(&opcode);
      for (std::size_t i(0); i < sizeof(opcode); ++i)
        p->push_back(s1[i]);

      if (g.sym->parametric())
      {
        const auto param(static_cast<std::int16_t>(g.par));
        assert(std::numeric_limits<decltype(param)>::min() <= g.par);
        assert(g.par <= std::numeric_limits<decltype(param)>::max());

        const auto *const s2 = reinterpret_cast<const unsigned char *>(&param);
        for (std::size_t i(0); i < sizeof(param); ++i)
          p->push_back(s2[i]);
      }
    }
  }

  ///
  /// \param[in] x second term of comparison.
  /// \return \c true if the two individuals are equal (symbol by symbol,
  ///         including introns).
  ///
  /// \note
  /// Age is not checked.
  ///
  bool i_num_ga::operator==(const i_num_ga &x) const
  {
    return signature_ == x.signature_ && genome_ == x.genome_;
  }

  ///
  /// \param[in] ind an individual to compare with \c this.
  /// \return a numeric measurement of the difference between \a ind and
  /// \c this (the number of different genes between individuals).
  ///
  unsigned i_num_ga::distance(const i_num_ga &ind) const
  {
    const auto cs(sset_->categories());

    unsigned d(0);
    for (auto i(decltype(cs){0}); i < cs; ++i)
      if (genome_[i] != ind.genome_[i])
        ++d;

    return d;
  }

  ///
  /// \param[in] verbose if \c true prints error messages to \c std::cerr.
  /// \return \c true if the individual passes the internal consistency check.
  ///
  bool i_num_ga::debug(bool verbose) const
  {
    const auto cs(sset_->categories());

    for (auto c(decltype(cs){0}); c < cs; ++c)
    {
      if (!genome_[c].sym)
      {
        if (verbose)
          std::cerr << k_s_debug << " Empty symbol pointer at position " << c
                    << '.' << std::endl;
        return false;
      }

      if (!genome_[c].sym->terminal())
      {
        if (verbose)
          std::cerr << k_s_debug << " Not-terminal symbol at position "
                    << c << '.' << std::endl;

        return false;
      }

      if (genome_[c].sym->category() != c)
      {
        if (verbose)
          std::cerr << k_s_debug << " Wrong category: " << c
                    << genome_[c].sym->display() << " -> "
                    << genome_[c].sym->category() << " should be " << c
                    << std::endl;
        return false;
      }

    }

    if (!signature_.empty() && signature_ != hash())
      return false;

    return env_->debug(verbose, true);
  }

  ///
  /// \param[out] s output stream.
  /// \param[in] ind individual to print.
  /// \return output stream including \a ind.
  ///
  std::ostream &operator<<(std::ostream &s, const i_num_ga &ind)
  {
    return ind.in_line(s);
  }
}  // namespace vita
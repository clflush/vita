/**
 *
 *  \file individual.h
 *
 *  \author Manlio Morini
 *  \date 2010/06/11
 *
 *  This file is part of VITA
 *
 */
  
#if !defined(INDIVIDUAL_H)
#define      INDIVIDUAL_H

#include <cmath>
#include <cstring>
#include <iomanip>
#include <list>
#include <set>

#include "vita.h"
#include "gene.h"

namespace vita
{

  class environment;

  ///
  /// \example example1.cc
  /// Creates a random individual and shows its content.
  /// \example example3.cc
  /// Performs three types of crossover between two random individuals. 
  ///
  class individual
  {
  public:
    individual() {};
    individual(const environment &, bool);

    void dump(std::ostream &) const;
    void list(std::ostream &) const;
    void tree(std::ostream &) const;
    void graphviz(std::ostream &, const std::string & = "") const;

    unsigned mutation();
    individual uniform_cross(const individual &) const;
    individual cross1(const individual &);
    individual cross2(const individual &);
    
    std::list<unsigned> blocks() const;
    individual destroy_block(unsigned) const;
    individual get_block(unsigned) const;
    individual replace(const symbol *const, 
                       const std::vector<unsigned> &) const;
    individual replace(const symbol *const, const std::vector<unsigned> &,
                       unsigned) const;

    void generalize(std::size_t, 
                    std::vector<unsigned> *, std::vector<symbol_t> *);

    individual compact(unsigned * = 0) const;
    individual optimize(unsigned * = 0, unsigned * = 0) const;
    unsigned normalize(individual &) const;

    bool operator==(const individual &) const;
    bool operator!=(const individual &x) const { return !(*this == x); };
    unsigned distance(const individual &) const;

    void pack(std::vector<boost::uint8_t> &) const;

    bool check() const;

    const gene &operator[](unsigned) const;
    unsigned size() const;
    unsigned eff_size() const;

    symbol_t type() const;

  private:
    static unsigned normalize(const individual &, 
                              const std::vector<unsigned> *, unsigned &,
                              individual &);
    void pack(std::vector<boost::uint8_t> &, unsigned) const;
    void tree(std::ostream &, unsigned, unsigned, unsigned) const;
    unsigned unpack(const std::vector<boost::uint8_t> &, unsigned);
    
    /// Active code in this individual (the best sequence of genes is starting
    /// here).
    unsigned          _best;

    const environment *_env;

    std::vector<gene> _code;

  public:
    class const_iterator
    {
    public:
      const_iterator(const individual &);
    
      bool operator()() const;
      
      unsigned operator++();
      
      const gene &operator*() const;
      const gene *operator->() const;
    
    private:
      const individual    &_ind;
      unsigned               _l;
      std::set<unsigned> _lines;
    };

    friend class const_iterator;
    friend class    interpreter;
  };

  std::ostream & operator<<(std::ostream &, const individual &);

  ///
  /// \return \c false when the iterator reaches the end.
  ///
  inline
  bool
  individual::const_iterator::operator()() const
  {
    return _l < _ind._code.size() && !_lines.empty();
  }

  ///
  /// \return reference to the current \c gene of the \c individual.
  ///
  inline
  const gene &
  individual::const_iterator::operator*() const
  {
    assert(_l < _ind._code.size());
    return _ind._code[_l];
  }

  ///
  /// \return pointer to the current \c gene of the \c individual.
  ///
  inline
  const gene *
  individual::const_iterator::operator->() const
  {
    assert(_l < _ind._code.size());
    return &_ind._code[_l];    
  }

  ///
  /// \param[in] i index of the \c gene of the \c individual.
  /// \return the i-th \c gene of the \c individual.
  ///
  inline
  const gene &
  individual::operator[](unsigned i) const
  {
    return _code[i];
  }
  
  ///
  /// \return the total size of the individual (effective size + introns).
  /// \see eff_size
  ///
  inline
  unsigned
  individual::size() const
  {
    return _code.size();
  }

  ///
  /// \return the effective size of the individual.
  /// \see size
  ///
  inline
  unsigned
  individual::eff_size() const
  {
    unsigned ef(0);

    for (const_iterator it(*this); it(); ++it)
      ++ef;

    return ef;
  }

  ///
  /// \param[in] locus location of the individual.
  /// \return an individual obtained from \c this choosing the best gene
  ///         sequence.
  ///
  inline
  individual
  individual::get_block(unsigned locus) const
  {
    individual ret(*this);

    ret._best = locus;

    assert(ret.check());
    return ret;
  }

  ///
  /// \param[out] p packed version (byte stream) if \c this individual.
  ///
  inline
  void
  individual::pack(std::vector<boost::uint8_t> &p) const 
  { 
    pack(p,_best); 
  }

}  // namespace vita

#endif  // INDIVIDUAL_H

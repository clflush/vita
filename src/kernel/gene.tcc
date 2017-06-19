/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2013-2017 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_GENE_H)
#  error "Don't include this file directly, include the specific .h instead"
#endif

#if !defined(VITA_GENE_TCC)
#define      VITA_GENE_TCC

///
/// A new gene built from terminal `t`.
///
/// \param[in] t a terminal
///
/// \note
/// This is usually called for filling the patch section of an individual.
///
template<unsigned K>
basic_gene<K>::basic_gene(const terminal &t) : sym(&t), args(t.arity())
{
  if (t.parametric())
    par = t.init();
}

///
/// Utility constructor to input hard-coded genomes.
///
/// \param[in] g a reference to a symbol and its arguments
///
/// With this constructor it is easy to write genome "by hand":
///     std::vector<gene> g(
///     {
///       {{f_add, {1, 2}}},  // [0] ADD 1,2
///       {{    y,     {}}},  // [1] Y
///       {{    x,     {}}}   // [2] X
///     };
///
template<unsigned K>
basic_gene<K>::basic_gene(const std::pair<symbol *, std::vector<index_t>> &g)
  : sym(g.first), args(g.first->arity())
{
  if (sym->parametric())
    par = terminal::cast(sym)->init();
  else
    std::transform(g.second.begin(), g.second.end(), args.begin(),
                   [](index_t i)
                   {
                     Expects(i <= std::numeric_limits<index_type>::max());

                     return static_cast<index_type>(i);
                   });
}

///
/// A new gene built from symbol `s` with argument in the `[from;sup[` range.
///
/// \param[in] s    a symbol
/// \param[in] from a starting index in the genome
/// \param[in] sup  an upper limit in the genome
///
/// \note
/// This is usually called for filling the standard section of an individual.
///
template<unsigned K>
basic_gene<K>::basic_gene(const symbol &s, index_t from, index_t sup)
  : sym(&s), args(s.arity())
{
  Expects(from < sup);

  if (s.parametric())
    par = terminal::cast(&s)->init();
  else
  {
    assert(sup <= std::numeric_limits<index_type>::max());

    std::generate(args.begin(), args.end(),
                  [from, sup]()
                  {
                    return random::between<index_type>(from, sup);
                  });
  }
}

///
/// \return the value, casted to type `T`, contained in this gene
///
/// \warning The method support only arithmetic / enum types.
///
template<unsigned K>
template<class T>
T basic_gene<K>::as() const
{
  static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                "basic_gene::as<T> requires an arithmetic / enum type");
  Expects(sym->parametric());
  return static_cast<T>(par);
}

///
/// \param[in] v value to be assigned to the gene
///
/// \warning The method support only arithmetic / enum types.
///
template<unsigned K>
template<class T>
void basic_gene<K>::operator=(T v)
{
  static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
                "Only arithmetic / enum types can be assigned to a gene");

  Expects(sym->parametric());
  copy_param(v,
             param_t<std::is_arithmetic<T>::value, std::is_enum<T>::value>());
}

///
/// Implements operator= for arithmetic types.
///
template<unsigned K>
template<class T>
void basic_gene<K>::copy_param(T v, arithmetic_p)
{
  par = static_cast<decltype(par)>(v);
}

///
/// Implements operator= for enum types.
///
template<unsigned K>
template<class T>
void basic_gene<K>::copy_param(T v, enum_p)
{
  par = static_cast<decltype(par)>(v.as_integer());
}

///
/// \param[in] i ordinal of an argument
/// \return      the locus that `i`-th argument of the current symbol refers to
///
template<unsigned K>
locus basic_gene<K>::arg_locus(unsigned i) const
{
  Expects(i < sym->arity());

  return {args[i], function::cast(sym)->arg_category(i)};
}

///
/// \param[in] g1 first term of comparison
/// \param[in] g2 second term of comparison
/// \return       `true` if `g1 == g2`
///
template<unsigned K>
bool operator==(const basic_gene<K> &g1, const basic_gene<K> &g2)
{
  if (g1.sym != g2.sym)
    return false;

  if (g1.sym->parametric())
  {
    assert(g1.sym->terminal());
    return almost_equal(g1.par, g2.par);
  }

  return std::equal(g1.args.begin(), g1.args.end(), g2.args.begin());
}

///
/// \param[in] g1 first term of comparison
/// \param[in] g2 second term of comparison
/// \return       `true` if `g1 != g2`
///
template<unsigned K>
bool operator!=(const basic_gene<K> &g1, const basic_gene<K> &g2)
{
  return !(g1 == g2);
}

///
/// \param[out] s output stream
/// \param[in]  g gene to print
/// \return       output stream including `g`
///
template<unsigned K>
std::ostream &operator<<(std::ostream &s, const basic_gene<K> &g)
{
  return s << (g.sym->parametric() ? terminal::cast(g.sym)->display(g.par)
                                   : g.sym->name());
}

#endif  // include guard

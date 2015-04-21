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

#include "kernel/symbol_set.h"
#include "kernel/adf.h"
#include "kernel/argument.h"
#include "kernel/random.h"

namespace vita
{
namespace
{
///
/// \param[in] symbols set of symbols.
/// \param[in] sum sum of the weights of the elements contained in `symbols`.
/// \return a random symbol from `symbols`.
///
/// Probably the fastest way to produce a realization of a random variable
/// X in a computer is to create a big table where each outcome _i_ is
/// inserted a number of times proportional to _P(X=i)_.
///
/// Two fast methods are described in "Fast Generation of Discrete Random
/// Variables" (Marsaglia, Tsang, Wang).
/// Also boost::random::discrete_distribution seems quite fast.
///
/// Anyway we choose the "roulette algorithm" because it's very simple and
/// allows changing weights dynamically (performance differences can hardly
/// be measured).
///
symbol *roulette_(const std::vector<symbol *> &symbols, unsigned sum)
{
  const auto slot(random::sup(sum));

  std::size_t i(0);
  for (auto wedge(symbols[i]->weight);
       wedge <= slot;
       wedge += symbols[++i]->weight)
  {}

  assert(i < symbols.size());
  return symbols[i];

  // The so called roulette-wheel selection via stochastic acceptance:
  //
  // for (;;)
  // {
  //   const symbol *s(random::element(symbols));
  //
  //   if (random::sup(max) < s->weight)
  //     return s;
  // }
  //
  // Internal tests have proved this is slower for Vita.

  // This is a different approach from Eli Bendersky
  // (<http://eli.thegreenplace.net/>):
  //
  //     unsigned total(0);
  //     std::size_t winner(0);
  //
  //     for (std::size_t i(0); i < symbols.size(); ++i)
  //     {
  //       total += symbols[i]->weight;
  //       if (random::sup(total + 1) < symbols[i]->weight)
  //         winner = i;
  //     }
  //     return winner;
  //
  // The interesting property of this algorithm is that you don't need to
  // know the sum of weights in advance in order to use it. The method is
  // cool, but slower than the standard roulette.
}
}  // namespace

///
/// Sets up the object.
/// The constructor allocates memory for up to `k_args` argument.
///
symbol_set::symbol_set() : arguments_(gene::k_args), symbols_(), all_(), by_()
{
  for (unsigned i(0); i < gene::k_args; ++i)
    arguments_[i] = make_unique<argument>(i);

  assert(debug(true));
}

///
/// \param[in] n index of an argument symbol.
/// \return a pointer to the n-th argument symbol.
///
symbol *symbol_set::arg(unsigned n) const
{
  assert(n < gene::k_args);
  return arguments_[n].get();
}

///
/// \param[in] i index of an ADT symbol.
/// \return a pointer to the i-th ADT symbol.
///
symbol *symbol_set::get_adt(unsigned i) const
{
  assert(i < all_.adt.size());
  return all_.adt[i];
}

///
/// \return the number of ADT functions stored.
///
unsigned symbol_set::adts() const
{
  return static_cast<unsigned>(all_.adt.size());
}

///
/// \param[in] i symbol to be added.
/// \return a raw pointer to the symbol just added (or `nullptr` in case of
///         error).
///
/// Adds a new symbol to the set. We manage to sort the symbols in
/// descending order, with respect to the weight, so the selection algorithm
/// would run faster.
///
symbol *symbol_set::insert(std::unique_ptr<symbol> i)
{
  assert(i);
  assert(i->weight);
  assert(i->debug());

  const auto raw(i.get());

  symbols_.push_back(std::move(i));

  all_.symbols.push_back(raw);
  all_.sum += raw->weight;

  if (raw->terminal())
  {
    all_.terminals.push_back(raw);

    if (raw->auto_defined())
      all_.adt.push_back(raw);
  }
  else  // not a terminal
    if (raw->auto_defined())
      all_.adf.push_back(raw);

  by_ = by_category(all_);

  std::sort(all_.symbols.begin(), all_.symbols.end(),
            [](const symbol *s1, const symbol *s2)
            { return s1->weight > s2->weight; });

  return raw;
}

///
void symbol_set::reset_adf_weights()
{
  for (auto adt : all_.adt)
  {
    const auto w(adt->weight);
    const auto delta(w >  1 ? w/2 :
                       w == 1 ?   1 : 0);
    all_.sum -= delta;
    adt->weight -= delta;

    if (delta && adt->weight == 0)
    {
      const auto opcode(adt->opcode());
      const auto adt_opcode([opcode](const symbol *s)
                            {
                              return s->opcode() == opcode;
                            });

      erase_if(all_.terminals, adt_opcode);
      erase_if(all_.symbols, adt_opcode);
    }
  }

  for (auto adf : all_.adf)
  {
    const auto w(adf->weight);
    const auto delta(w >  1 ? w/2 :
                       w == 1 ?   1 : 0);
    all_.sum -= delta;
    adf->weight -= delta;
  }

  by_ = by_category(all_);
}

///
/// \param[in] c a category.
/// \return a random terminal of category `c`.
///
symbol *symbol_set::roulette_terminal(category_t c) const
{
  assert(c < categories());

  return random::element(by_.category[c].terminals);
}

///
/// \param[in] c a category.
/// \return a random symbol of category `c`.
///
symbol *symbol_set::roulette(category_t c) const
{
  return roulette_(by_.category[c].symbols, by_.category[c].sum);
}

///
/// \return a random symbol from the set of all symbols.
///
symbol *symbol_set::roulette() const
{
  return roulette_(all_.symbols, all_.sum);
}

///
/// \param[in] opcode numerical code used as primary key for a symbol.
/// \return a pointer to the vita::symbol identified by `opcode`
///         (`nullptr` if not found).
///
symbol *symbol_set::decode(opcode_t opcode) const
{
  for (auto s : all_.symbols)
    if (s->opcode() == opcode)
      return s;

  return nullptr;
}

///
/// \param[in] dex the name of a symbol.
/// \return a pointer to the symbol identified by `dex` (0 if not found).
///
/// \attention Please note that opcodes (automatically assigned) are primary
/// keys for symbols. Conversely the name of a symbol is chosen by the
/// user, so, if you don't pay attention, different symbols may have the same
/// name.
///
symbol *symbol_set::decode(const std::string &dex) const
{
  assert(dex != "");

  for (auto s : all_.symbols)
    if (s->display() == dex)
      return s;

  return nullptr;
}

///
/// \return number of categories in the symbol set (>= 1).
///
/// See also data::categories().
///
unsigned symbol_set::categories() const
{
  return static_cast<unsigned>(by_.category.size());
}

///
/// \param[in] c a category.
/// \return number of terminals in category `c`.
///
unsigned symbol_set::terminals(category_t c) const
{
  assert(c < by_.category.size());
  return static_cast<unsigned>(by_.category[c].terminals.size());
}

///
/// \return `true` if there are enough terminals for secure individual
///         generation.
///
/// We want at least one terminal for every category.
///
bool symbol_set::enough_terminals() const
{
  std::set<category_t> need;

  for (const auto &sym : all_.symbols)
  {
    const auto arity(sym->arity());
    for (auto i(decltype(arity){0}); i < arity; ++i)
      need.insert(function::cast(sym)->arg_category(i));
  }

  for (const auto &i : need)
  {
    const collection &cc(by_.category[i]);

    if (i >= categories() || !cc.terminals.size())
      return false;
  }
  return true;
}

///
/// \param[out] o output stream.
/// \param[in] ss symbol set to be printed.
/// \return output stream including `ss`.
///
/// Useful for debugging purpose.
///
std::ostream &operator<<(std::ostream &o, const symbol_set &ss)
{
  for (const auto *s : ss.all_.symbols)
  {
    o << s->display();

    const auto arity(s->arity());
    if (arity)
      o << '(';
    for (auto j(decltype(arity){0}); j < arity; ++j)
      o << function::cast(s)->arg_category(j)
        << (j+1 == arity ? "" : ", ");
    if (arity)
      o << ')';

    o << " -> " << s->category() << " (opcode " << s->opcode()
      << ", parametric " << s->parametric()
      << ", weight " << s->weight << ")\n";
  }

  return o << "Sum: " << ss.all_.sum << '\n';
}

///
/// \param[in] verbose if `true` prints error messages to `std::cerr`.
/// \return `true` if the object passes the internal consistency check.
///
bool symbol_set::debug(bool verbose) const
{
  if (!all_.debug(verbose))
    return false;

  for (const collection &c : by_.category)
    if (!c.debug(verbose))
      return false;

  if (!enough_terminals())
  {
    if (verbose)
      std::cerr << k_s_debug << " Symbol set doesn't contain enough symbols\n";
    return false;
  }

  return true;
}

///
/// New empty collection.
///
symbol_set::collection::collection() : symbols(), terminals(), adf(), adt(),
                                       sum(0)
{
}

///
/// \param[in] verbose if `true` prints error messages to `std::cerr`.
/// \return `true` if the object passes the internal consistency check.
///
bool symbol_set::collection::debug(bool verbose) const
{
  decltype(sum) check_sum(0);

  auto print_head = [this]() -> std::ostream &
                    {
                      std::cerr << k_s_debug;
                      if (symbols.empty())
                        std::cerr << " Empty collection";
                      else
                        std::cerr << " Collection " << symbols[0]->category();

                      std::cerr << ": ";

                      return std::cerr;
                    };

  for (auto s : symbols)
  {
    if (!s->debug())
    {
      if (verbose)
        std::cerr << print_head() << "invalid symbol " << s->display() << "\n";
      return false;
    }

    check_sum += s->weight;

    if (s->weight == 0)
    {
      if (verbose)
        std::cerr << print_head() << "null weight for symbol " << s->display()
                  << "\n";
      return false;
    }

    if (s->terminal())
    {
      // Terminals must be in the terminals' vector.
      if (std::find(terminals.begin(), terminals.end(), s) ==
          terminals.end())
      {
        if (verbose)
          std::cerr << print_head() << "terminal " << s->display()
                    << " not correctly stored\n";
        return false;
      }

      if (s->auto_defined() &&
          std::find(adt.begin(), adt.end(), s) == adt.end())
      {
        if (verbose)
          std::cerr << print_head() << "automatic defined terminal "
                    << s->display() << " not correctly stored\n";
        return false;
      }
    }
    else  // Function
    {
      // Function must not be in the terminals' vector.
      if (std::find(terminals.begin(), terminals.end(), s) != terminals.end())
      {
        if (verbose)
          std::cerr << print_head() << "function " << s->display()
                    << " not correctly stored\n";
        return false;
      }

      if (s->auto_defined() &&
          std::find(adf.begin(), adf.end(), s) == adf.end())
      {
        if (verbose)
          std::cerr << print_head() << "automatic defined function "
                    << s->display() << " not correctly stored\n";
        return false;
      }
    }
  }

  if (check_sum != sum)
  {
    if (verbose)
      std::cerr << print_head() << "incorrect cached sum of weights (stored: "
                << sum << ", correct: " << check_sum << ")\n";
    return false;
  }

  const auto ssize(symbols.size());

  // The following condition should be met at the end of the symbol_set
  // specification.
  // Since we don't want to enforce a particular insertion order (e.g. terminals
  // before functions), we cannot perform the check here.
  //
  //     if (ssize && !terminals.size())
  //     {
  //       if (verbose)
  //         std::cerr << print_head() << "no terminal in the symbol set\n";
  //       return false;
  //     }

  if (ssize < terminals.size())
  {
    if (verbose)
      std::cerr << print_head()
                << "wrong terminal set size (less than symbol set)\n";
      return false;
  }

  if (ssize < adf.size())
  {
    if (verbose)
      std::cerr << print_head()
                << " wrong ADF set size (less than symbol set)\n";
    return false;
  }

  if (ssize < adt.size())
  {
    if (verbose)
      std::cerr << print_head()
                << " wrong ADT set size (less than symbol set)\n";
    return false;
  }

  return true;
}

///
/// \param[in] c a collection containing many categories of symbol.
///
/// Initialize the struct using collection `c` as input parameter (`c` should
/// be a collection containing more than one category).
///
symbol_set::by_category::by_category(const collection &c) : category()
{
  for (auto s : c.symbols)
  {
    const category_t cat(s->category());
    if (cat >= category.size())
    {
      category.resize(cat + 1);
      category[cat] = collection();
    }

    category[cat].symbols.push_back(s);
    category[cat].sum += s->weight;
  }

  for (auto t : c.terminals)
    category[t->category()].terminals.push_back(t);

  for (auto adf : c.adf)
    category[adf->category()].adf.push_back(adf);

  for (auto adt : c.adt)
    category[adt->category()].adt.push_back(adt);

  for (collection &coll : category)
    std::sort(coll.symbols.begin(), coll.symbols.end(),
              [](const symbol *s1, const symbol *s2)
              { return s1->weight > s2->weight; });

  assert(debug(true));
}

///
/// \param[in] verbose if `true` prints error messages to `std::cerr`.
/// \return `true` if the object passes the internal consistency check.
///
bool symbol_set::by_category::debug(bool verbose) const
{
  for (const collection &coll : category)
    if (!coll.debug(verbose))
      return false;

  return true;
}

}  // namespace vita

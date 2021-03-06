/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2016-2020 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_DATAFRAME_H)
#define      VITA_DATAFRAME_H

#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include "kernel/distribution.h"
#include "kernel/problem.h"
#include "kernel/src/category_set.h"

namespace vita
{
/// The type used as class id in classification tasks.
using class_t = std::size_t;

///
/// A 2-dimensional labeled data structure with columns of potentially
/// different types.
///
/// You can think of it like a spreadsheet or SQL table.
///
/// Dataframe:
/// - is modelled on the corresponding *pandas* object;
/// - is a forward iterable collection of "monomorphic" examples (all samples
///   have the same type and arity);
/// - accepts many different kinds of input: CSV and XRFF
///   (http://weka.wikispaces.com/XRFF) files.
///
class dataframe
{
public:
  // ---- Structures ----
  struct example;
  struct column;

  // ---- Aliases ----
  using examples_t = std::vector<example>;
  using value_type = typename examples_t::value_type;

  /// Raw input record.
  /// The ETL chain is:
  /// > FILE -> record_t -> example --(vita::push_back)--> vita::dataframe
  using record_t = std::vector<std::string>;

  /// A filter and transform function (returns `true` for records that should
  /// be loaded and, possibly, transform its input parameter).
  using filter_hook_t = std::function<bool (record_t &)>;

  // ---- Constructors ----
  dataframe();
  explicit dataframe(std::istream &, filter_hook_t = nullptr);
  explicit dataframe(const std::filesystem::path &, filter_hook_t = nullptr);

  // ---- Iterators ----
  using iterator = typename examples_t::iterator;
  using const_iterator = typename examples_t::const_iterator;
  using difference_type = typename examples_t::difference_type;

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;

  // ---- Modifiers ----
  void clear();
  iterator erase(iterator, iterator);

  // ---- Convenience ----
  std::size_t read(const std::filesystem::path &, filter_hook_t = nullptr);
  std::size_t read_csv(std::istream &, filter_hook_t = nullptr);
  std::size_t read_xrff(std::istream &, filter_hook_t = nullptr);
  bool operator!() const;

  void push_back(const example &);

  const category_set &categories() const;

  const column &get_column(unsigned) const;

  std::size_t size() const;
  bool empty() const;

  class_t classes() const;
  unsigned columns() const;
  unsigned variables() const;

  std::string class_name(class_t) const;

  bool debug() const;

private:
  bool read_record(const record_t &);
  example to_example(const record_t &, bool, bool);

  class_t encode(const std::string &);

  std::size_t read_csv(const std::filesystem::path &, filter_hook_t);
  std::size_t read_xrff(const std::filesystem::path &, filter_hook_t);
  std::size_t read_xrff(tinyxml2::XMLDocument &, filter_hook_t);

  void swap_category(category_t, category_t);

  // Integer are simpler to manage than textual data, so, when appropriate,
  // input strings are converted into integers by this map and the `encode`
  // static function.
  std::map<std::string, class_t> classes_map_;

  // How is the dataset organized? Sometimes we have a dataset header (XRFF
  // file format), other times it has to be implicitly derived (e.g. CSV).
  // `header_[0]` is the output column (it contains informations about
  // problem's output).
  std::vector<column> header_;

  // What are the categories we are dealing with?
  // Note: `category_[0]` is the output category.
  category_set categories_;

  // Available data.
  examples_t dataset_;
};

domain_t from_weka(const std::string &);

///
/// Stores a single element of the dataset.
///
/// The `struct` consists of an input vector (`input`) and an answer value
/// (`output`). Depending on the kind of problem, `output` stores:
/// * a numeric value (symbolic regression problem);
/// * a categorical value (classification problem).
///
/// `difficulty` and `age` are parameters used by the Dynamic Subset
/// Selection algorithm (see "Dynamic Training Subset Selection for
/// Supervised Learning in Genetic Programming" - Chris Gathercole, Peter
/// Ross).
///
struct dataframe::example
{
  /// The thing about which we want to make a prediction (aka instance). The
  /// elements of the vector are features.
  std::vector<value_t> input = {};
  /// The answer for the prediction task either the answer produced by the
  /// machine learning system, or the right answer supplied in the training
  /// data.
  value_t             output = {};

  std::uintmax_t difficulty  = 0;
  unsigned              age  = 0;

  void clear() { *this = example(); }
};

///
/// Gets the `class_t` ID (aka label) for a given example.
///
/// \param[in] e an example
/// \return      the label of `e`
///
/// \warning Used only in classification tasks.
///
inline class_t label(const dataframe::example &e)
{
  Expects(std::holds_alternative<D_INT>(e.output));
  return std::get<D_INT>(e.output);
}

///
/// Get the output value for a given example in a symbolic regression task.
///
/// \param[in] e an example
/// \return      the output value for example  `e`
///
///
template<class T>
T label_as(const dataframe::example &e)
{
  const auto &v(e.output);

  if (std::holds_alternative<D_DOUBLE>(v))
    return static_cast<T>(std::get<D_DOUBLE>(v));

  if (std::holds_alternative<D_INT>(v))
    return static_cast<T>(std::get<D_INT>(v));

  return 0;
}

///
/// Informations about a "column" of the dataset.
///
struct dataframe::column
{
  std::string       name;
  category_t category_id;
};

}  // namespace vita

#endif  // include guard

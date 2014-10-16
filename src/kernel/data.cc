/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2014 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#include <algorithm>
#include <vector>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "kernel/data.h"
#include "kernel/random.h"
#include "kernel/symbol.h"

namespace vita
{

namespace
{
///
/// \param[in] s the string to be converted.
/// \param[in] d what type should \a s be converted in?
/// \return the converted data.
///
///     convert("123.1", sym_double) == any(123.1f)
///
/// \exception std::invalid_argument
/// if no conversion could be performed.
///
any convert(const std::string &s, domain_t d)
{
  switch (d)
  {
  case domain_t::d_bool:   return any(std::stoi(s));
  case domain_t::d_int:    return any(std::stoi(s));
  case domain_t::d_double: return any(std::stod(s));
  case domain_t::d_string: return            any(s);
  default:                 return             any();
  }
}

///
/// \param[in] s the string to be tested.
/// \return \c true if \a s contains a number.
///
bool is_number(const std::string &s)
{
  try
  {
    std::stod(s);
  }
  catch(std::invalid_argument &)
  {
    return false;
  }

  return true;
}
}  // namespace

///
/// New empty data instance.
///
data::data() : classes_map_(), header_(), categories_(),
               dataset_(k_sup_dataset), slice_(k_sup_dataset, 0),
               active_dataset_(training)
{
  assert(debug());
}

///
/// \param[in] filename name of the file containing the learning collection.
/// \param[in] verbosity verbosity level (see environment::verbosity for
///            further details).
///
/// New data instance containing the learning collection from \a filename.
///
data::data(const std::string &filename, unsigned verbosity) : data()
{
  assert(!filename.empty());

  open(filename, verbosity);

  assert(debug());
}

///
/// Resets the object.
///
void data::clear()
{
  *this = data();
}

///
/// \param[in] d the active dataset.
///
/// We can choose the data we want to operate on (training / validation /
/// test set).
///
void data::dataset(dataset_t d)
{
  active_dataset_ = d;
}

///
/// \return the type (training, validation, test) of the active dataset.
///
data::dataset_t data::dataset() const
{
  return active_dataset_;
}

///
/// \param[in] n number of elements for the slice.
///
/// Cuts a 'slice' of \a n elements in the active dataset. Future calls to
/// data::end() will refer to the active slice (a subset of the dataset).
/// To reset the slice call data::slice with argument 0.
///
void data::slice(std::size_t n)
{
  slice_[dataset()] = n;
}

///
/// \return reference to the first element of the active dataset.
///
data::iterator data::begin()
{
  return dataset_[dataset()].begin();
}

///
/// \return a constant reference to the first element of the active dataset.
///
data::const_iterator data::begin() const
{
  return dataset_[dataset()].begin();
}

///
/// \return a reference to the last+1 (sentry) element of the active
///         dataset.
///
data::iterator data::end()
{
  const auto n(slice_[dataset()]);

  if (n == 0 || n > size())
    return dataset_[dataset()].end();

  return std::next(dataset_[dataset()].begin(),
                   static_cast<decltype(dataset_)::difference_type>(n));
}

///
/// \return a constant reference to the last+1 (sentry) element of the active
///         dataset.
///
data::const_iterator data::end() const
{
  const auto n(slice_[dataset()]);

  if (n == 0 || n > size())
    return dataset_[dataset()].end();

  return std::next(dataset_[dataset()].begin(),
                   static_cast<decltype(dataset_)::difference_type>(n));
}

///
/// \param[in] d a dataset (training / validation / test set).
/// \return the size of the dataset \a d.
///
/// \note
/// Please note that the result is independent of the active slice.
///
std::size_t data::size(dataset_t d) const
{
  return dataset_[d].size();
}

///
/// \return the size of the active dataset.
///
std::size_t data::size() const
{
  return size(dataset());
}

///
/// \return a const reference to the set of categories associated with the
///         dataset.
///
const category_set &data::categories() const
{
  return categories_;
}

///
/// \param[in] i index of a column.
/// \return a const reference to the i-th column of the dataset.
///
const data::column &data::get_column(unsigned i) const
{
  assert(i < columns());
  return header_[i];
}

///
/// \param[in] f the comparer used for sorting.
///
/// Sorts the active slice in the current dataset (slice is preserved).
///
void data::sort(std::function<bool (const example &, const example &)> f)
{
  const auto partition_size(static_cast<std::size_t>(
                              std::distance(begin(), end())));

  auto &d(dataset_[dataset()]);
  std::sort(d.begin(), d.end(), f);

  slice(partition_size);
}

///
/// \param[in] percentage \a size_of_validation_set / \a size_of_dataset
///                       ratio.
///
/// Splits the dataset in two subsets (training set, validation set)
/// according to the \a r ratio.
///
/// \attention
/// The procedure resets active slices.
///
void data::partition(unsigned percentage)
{
  assert(percentage < 100);

  if (!percentage)
    return;

  // Validation set items are moved to the training set.
  std::move(dataset_[validation].begin(), dataset_[validation].end(),
            std::back_inserter(dataset_[training]));
  dataset_[validation].clear();

  auto available(dataset_[training].size());
  if (available && percentage)
  {
    using diff_t = decltype(dataset_)::difference_type;
    const auto needed(static_cast<diff_t>(available * percentage / 100));

    std::shuffle(dataset_[training].begin(), dataset_[training].end(),
                 random::engine());

    std::move(std::prev(dataset_[training].end(), needed),
              dataset_[training].end(),
              std::back_inserter(dataset_[validation]));

    dataset_[training].erase(std::prev(dataset_[training].end(), needed),
                             dataset_[training].end());

    /*
    // This algorithm hint is due to Kyle Cronin:
    //
    // > Iterate through and for each element make the probability of
    // >  selection = (number needed)/(number left)
    // >
    // > So if you had 40 items, the first would have a 5/40 chance of being
    // > selected. If it is, the next has a 4/39 chance, otherwise it has a
    // > 5/39 chance. By the time you get to the end you will have your 5
    // > items, and often you'll have all of them before that.
    auto iter(dataset_[training].begin());
    while (needed)
    {
      assert(available);

      if (needed == available ||
          random::boolean(static_cast<double>(needed) /
                          static_cast<double>(available)))
      {   // selected
        dataset_[validation].push_back(*iter);
        iter = dataset_[training].erase(iter);

        --needed;
      }
      else  // not selected
        ++iter;

      assert(dataset_[training].begin() <= iter);
      assert(iter < dataset_[training].end());

      --available;
    }

    assert(!needed);
    */

    std::fill(slice_.begin(), slice_.end(), 0);
  }
}

///
/// \return number of columns of the dataset.
///
/// \note data class supports just one output for every instance, so, if
/// the dataset is not empty: \code variables() + 1 == columns() \endcode.
///
unsigned data::columns() const
{
  assert(dataset_[dataset()].empty() || variables() + 1 == header_.size());

  return static_cast<unsigned>(header_.size());
}

///
/// \return number of classes of the problem (== 0 for a symbolic regression
///         problem, > 1 for a classification problem).
///
unsigned data::classes() const
{
  return static_cast<unsigned>(classes_map_.size());
}

///
/// \return input vector dimension.
///
/// \note data class supports just one output for every instance, so, if
/// the dataset is not empty, \code variables() + 1 == columns() \endcode.
///
unsigned data::variables() const
{
  const auto n(dataset_[dataset()].empty()
               ? 0u : static_cast<unsigned>(begin()->input.size()));

  assert(dataset_[dataset()].empty() || n + 1 == header_.size());

  return n;
}

///
/// \param[in] label name of a class of the learning collection.
/// \return the (numerical) tag associated with class \a label.
///
class_t data::encode(const std::string &label)
{
  if (classes_map_.find(label) == classes_map_.end())
  {
    const auto n(static_cast<category_t>(classes_map_.size()));
    classes_map_[label] = n;
  }

  return classes_map_[label];
}

///
/// \param[in] i the encoded (data::encode()) value of a class.
/// \return the name of the class encoded with the \c unsigned \a i (or an
///         empty string if such class cannot be find).
///
/// \note
/// Boost Bitmap could be used to speed up the search in \a classes_map_, but
/// to date speed isn't an issue.
///
std::string data::class_name(class_t i) const
{
  for (const auto &p : classes_map_)
    if (p.second == i)
      return p.first;

  return "";
}

///
/// \param[in] line line to be parsed.
/// \param[in] delimiter separator character for fields.
/// \param[in] trim if \c true trims leading and trailing spaces adjacent to
///                 commas (this practice is contentious and in fact is
///                 specifically prohibited by RFC 4180, which states,
///                 "Spaces are considered part of a field and should not be
///                 ignored."
/// \return a vector where each element is a field of the CSV line.
///
/// This function parses a line of data by a delimiter. If you pass in a
/// comma as your delimiter it will parse out a Comma Separated Value (CSV)
/// file. If you pass in a '\t' char it will parse out a tab delimited file
/// (.txt or .tsv). CSV files often have commas in the actual data, but
/// accounts for this by surrounding the data in quotes. This also means the
/// quotes need to be parsed out, this function accounts for that as well.
/// The only (known) problem with this code, is that the definition of a csv
/// (http://en.wikipedia.org/wiki/Comma-separated_values) allows for the
/// newline character '\n' to be part of a csv field if the field is
/// surrounded by quotes. The csvline function takes care of this properly,
/// but the data::open function, which calls csvline, doesn't handle it. Most
/// CSV files do not have a \n in the middle of the field, so it is usually
/// not worth worrying about.
/// This is a slightly modified version of the function at
/// <http://www.zedwood.com/article/112/cpp-csv-parser>.
/// Escaped List Separator class from Boost C++ libraries is also very nice
/// and efficient for parsing, but it is not as easily applied.
///
std::vector<std::string> data::csvline(const std::string &line, char delimiter,
                                       bool trim)
{
  std::vector<std::string> record;  // the return value

  const char quote('"');

  bool inquotes(false);
  std::string curstring;

  for (auto length(line.length()), pos(decltype(length){0});
       pos < length && line[pos];)
  {
    const char c(line[pos]);

    if (!inquotes && !curstring.length() && c == quote)  // begin quote char
      inquotes = true;
    else if (inquotes && c == quote)
    {
      if (pos + 1 < length && line[pos + 1] == quote)  // quote char
      {
        // Encountered 2 double quotes in a row (resolves to 1 double quote).
        curstring.push_back(c);
        ++pos;
      }
      else  // end quote char
        inquotes = false;
    }
    else if (!inquotes && c == delimiter)  // end of field
    {
      record.push_back(curstring);
      curstring = "";
    }
    else if (!inquotes && (c == '\r' || c == '\n'))
      break;
    else
      curstring.push_back(c);

    ++pos;
  }

  assert(!inquotes);

  record.push_back(curstring);

  if (trim)
    for (auto size(record.size()), i(decltype(size){0}); i < size; ++i)
      boost::trim(record[i]);

  return record;
}

///
/// \param[in] c1 a category.
/// \param[in] c2 a category.
///
/// Swap categories \a c1 and \a c2, updating the \a header_ and
/// \a categories_ vector.
///
void data::swap_category(category_t c1, category_t c2)
{
  const auto n_col(columns());

  assert(c1 < n_col);
  assert(c2 < n_col);

  categories_.swap(c1, c2);

  for (auto i(decltype(n_col){0}); i < n_col; ++i)
    if (header_[i].category_id == c1)
      header_[i].category_id = c2;
    else if (header_[i].category_id == c2)
      header_[i].category_id = c1;
}

///
/// \param[in] filename the xrff file.
/// \return number of lines parsed (0 in case of errors).
///
/// Loads the content of \a filename into the active dataset.
///
/// \note
/// An XRFF (eXtensible attribute-Relation File Format) file describes a list
/// of instances sharing a set of attributes.
/// The original format is defined in http://weka.wikispaces.com/XRFF, we
/// extend it with an additional (non-standard) feature: attribute category.
///
///     <attribute name="vehicle length" type="numeric" category="length" /
///     <attribute name="vehicle width" type="numeric" category="length" />
///     <attribute name="vehicle weight" type="numeric" category="weight" />
///
/// This feature is used to constrain the search (Strongly Typed Genetic
/// Programming).
///
/// \post
/// * \a header_[0] is the output column (it contains informations about
///   problem's output);
/// * \a category(0) is the output category (for symbolic regression
///   problems it is the output type of the xrff file, for classification
///   problems it is the \a numeric type).
///
/// \warning
/// To date:
/// * we don't support compressed XRFF files;
/// * XRFF files cannot be used to load test set (problems with missing
///   output column and possible column category redefinition).
///
/// \note
/// Test set can have an empty output value.
///
std::size_t data::load_xrff(const std::string &filename)
{
  assert(dataset() == training);

  using boost::property_tree::ptree;
  using boost::property_tree::read_xml;

  ptree pt;
  read_xml(filename, pt);

  unsigned n_output(0);
  bool classification(false);

  // Iterate over dataset.header.attributes selection and store all found
  // attributes in the header vector. The get_child() function returns a
  // reference to the child at the specified path; if there is no such child
  // IT THROWS.
  for (auto dha : pt.get_child("dataset.header.attributes"))
    if (dha.first == "attribute")
    {
      bool output(false);

      column a;

      // In a ptree xml-attributes are just sub-elements of a special element
      // <xmlattr>.
      // Note that we don't have to provide the template parameter when it is
      // deduced from the default value.
      a.name = dha.second.get("<xmlattr>.name", "");

      // One can define which attribute should act as output value via the
      // class="yes" attribute in the attribute specification in the header.
      output = dha.second.get("<xmlattr>.class", "no") == "yes";

      auto xml_type(dha.second.get("<xmlattr>.type", ""));
      auto category_name(dha.second.get("<xmlattr>.category", xml_type));

      if (output)
      {
        ++n_output;

        // We can manage only one output column.
        if (n_output > 1)
          return 0;

        // For classification problems we use discriminant functions, so the
        // actual output type is always numeric.
        classification = (xml_type == "nominal" || xml_type == "string");
        if (classification)
        {
          xml_type = "numeric";
          category_name = "numeric";
        }
      }

      a.category_id = categories_.insert({category_name, from_weka(xml_type),
                                          {}});

      if (xml_type == "nominal")
        try
        {
          // Store label1... labelN.
          for (auto l : dha.second.get_child("labels"))
            if (l.first == "label")
              categories_.add_label(a.category_id, l.second.data());
        }
        catch(...)
        {
        }

      // Output column is always the first one.
      if (output)
        header_.insert(header_.begin(), a);
      else
        header_.push_back(a);
    }

  // XRFF needs informations about the columns.
  if (!columns())
    return 0;

  // If no output column is specified the default XRFF output column is the
  // last one (and it is the first element of the header_ vector).
  if (n_output == 0)
  {
    header_.insert(header_.begin(), header_.back());
    header_.pop_back();
  }

  // Category 0 is the output category.
  swap_category(category_t(0), header_[0].category_id);

  unsigned parsed(0);
  for (auto bi : pt.get_child("dataset.body.instances"))
    if (bi.first == "instance")
    {
      example instance;

      unsigned index(0);
      for (auto v(bi.second.begin()); v != bi.second.end(); ++v, ++index)
        if (v->first == "value")
          try
          {
            const auto domain(
              categories_.find(header_[index].category_id).domain);

            const std::string value(v->second.data());

            if (index == 0)  // output value
            {
              // Strings could be used as label for classes, but integers
              // are simpler and faster to manage (arrays instead of maps).
              if (classification)
                instance.output = encode(value);
              else
              {
                instance.output = convert(value, domain);
                instance.d_output = domain;
              }
            }
            else  // input value
              instance.input.push_back(convert(value, domain));
          }
          catch(std::invalid_argument &)
          {
            instance.clear();
            continue;
          }

      if (instance.input.size() + 1 == columns())
      {
        dataset_[dataset()].push_back(instance);
        ++parsed;
      }
    }

  return debug() ? parsed : 0;
}

///
/// \param[in] filename the csv file.
/// \param[in] verbosity verbosity level (see environment::verbosity for
///            further details).
/// \return number of lines parsed (0 in case of errors).
///
/// We follow the Google Prediction API convention
/// (https://developers.google.com/prediction/docs/developer-guide?hl=it#data-format):
/// * NO HEADER ROW is allowed;
/// * only one example is allowed per line. A single example cannot contain
///   newlines and cannot span multiple lines;
/// * columns are separated by commas. Commas inside a quoted string are not
///   column delimiters;
/// * THE FIRST COLUMN REPRESENTS THE VALUE (numeric or string) for that
///   example. If the first column is numeric, this model is a REGRESSION
///   model; if the first column is a string, it is a CATEGORIZATION
///   (i.e. classification) model. Each column must describe the same kind of
///   information for that example;
/// * the column order of features in the table does not weight the results;
///   the first feature is not weighted any more than the last;
/// * as a best practice, remove punctuation (other than apostrophes) from
///   your data. This is because commas, periods, and other punctuation
///   rarely add meaning to the training data, but are treated as meaningful
///   elements by the learning engine. For example, "end." is not matched to
///   "end";
/// * TEXT STRINGS:
///   * place double quotes around all text strings;
///   * text matching is case-sensitive: "wine" is different from "Wine.";
///   * if a string contains a double quote, the double quote must be escaped
///     with another double quote, for example:
///     "sentence with a ""double"" quote inside";
/// * NUMERIC VALUES:
///   * both integer and decimal values are supported;
///   * numbers in quotes without whitespace will be treated as numbers, even
///     if they are in quotation marks. Multiple numeric values within
///     quotation marks in the same field will be treated as a string. For
///     example:
///       Numbers: "2", "12", "236"
///       Strings: "2 12", "a 23"
///
/// \note
/// Test set can have an empty output value.
///
std::size_t data::load_csv(const std::string &filename, unsigned verbosity)
{
  std::ifstream from(filename.c_str());
  if (!from)
    return 0;

  bool classification(false), format(columns());

  std::string line;
  while (std::getline(from, line))
  {
    const auto record(csvline(line));
    const auto fields(record.size());

    // If we don't know the dataset format, the first line will be used to
    // learn it.
    if (!format)
    {
      classification = !is_number(record[0]);

      header_.reserve(fields);

      for (auto field(decltype(fields){0}); field < fields; ++field)
      {
        assert(!size());  // If we have data then data format must be known

        std::string s_domain(is_number(record[field])
                             ? "numeric"
                             : "string" + std::to_string(field));

        // For classification problems we use discriminant functions, so the
        // actual output type is always numeric.
        if (field == 0 && classification)
          s_domain = "numeric";

        const domain_t domain(s_domain == "numeric" ? domain_t::d_double
                                                    : domain_t::d_string);

        const category_t tag(categories_.insert({s_domain, domain, {}}));

        header_.push_back({"", tag});
      }

      format = true;
    }  // if (!format)

    if (fields == columns())
    {
      example instance;

      for (auto field(decltype(fields){0}); field < fields; ++field)
        try
        {
          const category_t c(header_[field].category_id);
          const domain_t d(categories_.find(c).domain);

          const auto value(record[field]);

          if (field == 0)  // output value
          {
            if (value.empty())
            {
              assert(dataset() == test);
              // For test set the output class/value could be missing (e.g.
              // for kaggle.com competition test set).
            }
            else
            {
              if (classification)
                instance.output = encode(value);
              else
              {
                instance.output = convert(value, d);
                instance.d_output = d;
              }
            }
          }
          else  // input value
          {
            instance.input.push_back(convert(value, d));
            if (d == domain_t::d_string)
              categories_.add_label(c, value);
          }
        }
        catch(std::invalid_argument &)
        {
          instance.clear();
          continue;
        }

      if (instance.input.size() + 1 == columns())
        dataset_[dataset()].push_back(instance);
      else if (verbosity >= 2)
        std::cout << k_s_warning << " [" << line << "] skipped" << std::endl;
    }
  }

  return debug() ? size() : static_cast<std::size_t>(0);
}

///
/// \param[in] f name of the file containing the data set.
/// \param[in] verbosity verbosity level (see environment::verbosity for
///            further details).
/// \return number of lines parsed (0 in case of errors).
///
/// Loads the content of \a f into the active dataset.
///
/// \warning
/// * Training/valutation set must be loaded before test set.
/// * Before changing problem the data object should be clear.
///
/// So:
///
///     dataset(training);
///     open("training.csv");
///     dataset(test);
///     open("test.csv");
///     ...
///     clear();
///     dataset(training);
///     open("training2.csv");
///     ...
///
/// \note
/// Test set can have an empty output value.
///
std::size_t data::open(const std::string &f, unsigned verbosity)
{
  const bool xrff(boost::algorithm::iends_with(f, ".xrff") ||
                  boost::algorithm::iends_with(f, ".xml"));

  return xrff ? load_xrff(f) : load_csv(f, verbosity);
}

///
/// \return \c true if the current dataset is empty.
///
bool data::operator!() const
{
  return size() == 0;
}

///
/// \return \c true if the object passes the internal consistency check.
///
bool data::debug() const
{
  const auto cl_size(classes());
  // If this is a classification problem then there should be at least two
  // classes.
  if (cl_size == 1)
    return false;

  for (const auto &d : dataset_)
    if (!d.empty() && &d != &dataset_[test])
    {
      const auto in_size(d.begin()->input.size());

      for (const auto &e : d)
      {
        if (e.input.size() != in_size)
          return false;

        if (cl_size && e.tag() >= cl_size)
          return false;
      }
    }

  return true;
}

///
/// \param[in] n the name of a weka domain.
/// \return the internel id of the weka-domain \a n (\c d_void if it's
///         unknown or not managed).
///
domain_t data::from_weka(const std::string &n)
{
  static const std::map<const std::string, domain_t> map(
  {
    // This type is vita-specific (not standard).
    {"boolean", domain_t::d_bool},

    {"integer", domain_t::d_int},

    // Real and numeric are treated as double precision number (d_double).
    {"numeric", domain_t::d_double},
    {"real", domain_t::d_double},

    // Nominal values are defined by providing a list of possible values.
    {"nominal", domain_t::d_string},

    // String attributes allow us to create attributes containing arbitrary
    // textual values. This is very useful in text-mining applications.
    {"string", domain_t::d_string}

    // {"date", ?}, {"relational", ?}
  });

  const auto &i(map.find(n));
  return i == map.end() ? domain_t::d_void : i->second;
}
}  // namespace vita
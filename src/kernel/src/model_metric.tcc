/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2015-2019 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_SRC_MODEL_METRIC_H)
#  error "Don't include this file directly, include the specific .h instead"
#endif

#if !defined(VITA_SRC_MODEL_METRIC_TCC)
#define      VITA_SRC_MODEL_METRIC_TCC

///
/// \param[in] l the model whose accuracy we are evaluating
/// \param[in] d a dataset
/// \return      the accuracy
///
template<class T>
double accuracy_metric<T>::operator()(const core_reg_lambda_f<T> *l,
                                      const dataframe &d) const
{
  Expects(!d.classes());
  Expects(d.begin() != d.end());

  std::uintmax_t ok(0), total_nr(0);

  for (const auto &example : d)
  {
    const any res((*l)(example));
    if (res.has_value() &&
        issmall(to<number>(res) - label_as<number>(example)))
      ++ok;

    ++total_nr;
  }

  Ensures(total_nr);
  return static_cast<double>(ok) / static_cast<double>(total_nr);
}

///
/// \param[in] l the model whose accuracy we are evaluating
/// \param[in] d a dataset
/// \return      the accuracy
///
template<class T>
double accuracy_metric<T>::operator()(const core_class_lambda_f<T> *l,
                                      const dataframe &d) const
{
  Expects(d.classes());
  Expects(d.begin() != d.end());

  std::uintmax_t ok(0), total_nr(0);

  for (const auto &example : d)
  {
    if (l->tag(example).first == label(example))
      ++ok;

    ++total_nr;
  }

  Ensures(total_nr);
  return static_cast<double>(ok) / static_cast<double>(total_nr);
}

#endif  // include guard

/**
 *
 *  \file src_evaluator.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011, 2012 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "src_evaluator.h"
#include "individual.h"
#include "src_interpreter.h"

namespace vita
{
  ///
  /// \param[in] d pointer to data that the evaluator will use.
  ///
  src_evaluator::src_evaluator(data *d) : dat_(d)
  {
    assert(d);
  }

  ///
  /// \param[in] ind program used for fitness evaluation.
  /// \return the fitness (greater is better, max is 0) and the accuracy
  ///         (percentage).
  ///
  score_t sum_of_errors_evaluator::operator()(const individual &ind)
  {
    assert(!dat_->classes());
    assert(dat_->cbegin() != dat_->cend());

    src_interpreter agent(ind);

    double err(0.0);
    int illegals(0);
    unsigned ok(0), total_nr(0);

    for (data::iterator t(dat_->begin()); t != dat_->end(); ++t)
    {
      err += error(agent, t, &illegals, &ok);

      ++total_nr;
    }

    assert(total_nr);

    // Note that we take the average error: this way fast() and operator()
    // outputs can be compared.
    return score_t(fitness_t(-err / total_nr),
                   static_cast<double>(ok) / static_cast<double>(total_nr));
  }

  ///
  /// \param[in] ind program used for fitness evaluation.
  /// \return the fitness (greater is better, max is 0) and the accuracy
  ///         (percentage).
  ///
  /// This function is similar to operator()() but will skip 3 out of 4
  /// training instances, so it's faster ;-)
  ///
  score_t sum_of_errors_evaluator::fast(const individual &ind)
  {
    assert(!dat_->classes());
    assert(dat_->cbegin() != dat_->cend());

    src_interpreter agent(ind);

    double err(0.0);
    int illegals(0);
    unsigned ok(0), total_nr(0);
    unsigned counter(0);

    for (data::iterator t(dat_->begin()); t != dat_->end(); ++t)
      if (dat_->size() <= 20 || (counter++ % 5) == 0)
      {
        err += error(agent, t, &illegals, &ok);

        ++total_nr;
      }

    assert(total_nr);

    // Note that we take the average error: this way fast() and operator()
    // outputs can be compared.
    return score_t(fitness_t(-err / total_nr),
                   static_cast<double>(ok) / static_cast<double>(total_nr));
  }

  ///
  /// \param[in] agent interpreter used for the evaluation of the current
  ///                  individual. Note that this isn't a constant reference
  ///                  because the internal state of agent changes during
  ///                  evaluation; anyway this is an input-only parameter.
  /// \param[in] t the current training case.
  /// \param[in,out] illegals number of illegals values found evaluating the
  ///                         current individual so far.
  /// \param[in,out] ok corret answers of the current individual so far.
  /// \return a measurement of the error of the current individual on the
  ///         training case \a t.
  ///
  double sae_evaluator::error(src_interpreter &agent, data::iterator t,
                              int *const illegals, unsigned *const ok)
  {
    const any res(agent.run(*t));

    double err;
    if (res.empty())
      err = std::pow(100.0, ++(*illegals));
    else
      err = std::fabs(interpreter::to_double(res) -
                      data::cast<double>(t->output));

    if (err > 0.1)
      ++t->difficulty;
    else
      ++(*ok);

    return err;
  }

  ///
  /// \param[in] agent interpreter used for the evaluation of the current
  ///                  individual. Note that this isn't a constant reference
  ///                  because the internal state of agent changes during
  ///                  evaluation; anyway this is an input-only parameter.
  /// \param[in] t the current training case.
  /// \param[in,out] illegals number of illegals values found evaluating the
  ///                         current individual so far.
  /// \param[in,out] ok corret answers of the current individual so far.
  /// \return a measurement of the error of the current individual on the
  ///         training case \a t.
  ///
  double sse_evaluator::error(src_interpreter &agent, data::iterator t,
                              int *const illegals, unsigned *const ok)
  {
    const any res(agent.run(*t));
    double err;
    if (res.empty())
      err = std::pow(100.0, ++(*illegals));
    else
    {
      err = interpreter::to_double(res) - data::cast<double>(t->output);
      err *= err;
    }

    if (err > 0.1)
      ++t->difficulty;
    else
      ++(*ok);

    return err;
  }

  ///
  /// \param[in] agent interpreter used for the evaluation of the current
  ///                  individual. Note that this isn't a constant reference
  ///                  because the internal state of agent changes during
  ///                  evaluation; anyway this is an input-only parameter.
  /// \param[in] t the current training case.
  /// \param[in,out]
  /// \param[in,out] ok corret answers of the current individual so far.
  /// \return a measurement of the error of the current individual on the
  ///         training case \a t.
  ///
  double count_evaluator::error(src_interpreter &agent, data::iterator t,
                                int *const, unsigned *const ok)
  {
    const any res(agent.run(*t));

    const bool err(res.empty() ||
                   std::fabs(interpreter::to_double(res) -
                             data::cast<double>(t->output)) >=
                   float_epsilon);

    if (err)
      ++t->difficulty;
    else
      ++(*ok);

    return err ? 1.0 : 0.0;
  }

  ///
  /// \param[in] d training data.
  /// \param[in] x_slot basic parameter for the Slotted Dynamic Class Boundary
  ///                   Determination algorithm.
  ///
  dyn_slot_evaluator::dyn_slot_evaluator(data *d, size_t x_slot)
    : src_evaluator(d), x_slot_(x_slot)
  {
    assert(d);
    assert(x_slot);
  }

  ///
  /// \param[in] x the numeric value that should be mapped in the [0,1]
  ///              interval.
  /// \return a double in the [0,1] range.
  ///
  /// This is a sigmoid function (it is a bounded real function, "S" shaped,
  /// with positive derivative everywhere).
  ///
  /// \see
  /// <http://en.wikipedia.org/wiki/Sigmoid_function>
  ///
  inline
  double dyn_slot_evaluator::normalize_01(double x)
  {
    // return (1.0 + x / (1 + std::fabs(x))) / 2.0;  // Algebraic function.

    return 0.5 + std::atan(x) / 3.1415926535;        // Arctangent.

    // return 0.5 + std::tanh(x);                    // Hyperbolic tangent.

    // return 1.0 / (1.0 + std::exp(-x));            // Logistic function.
  }

  ///
  /// \return number of slots used for the classification task.
  ///
  size_t dyn_slot_evaluator::n_slots() const
  {
    return dat_->classes() * x_slot_;
  }

  ///
  /// \param[in] ind individual used for classification.
  /// \param[in] e input data for \a ind.
  /// \return the slot the input instance falls into.
  ///
  size_t dyn_slot_evaluator::slot(const individual &ind,
                                  const data::example &e)
  {
    assert(ind.check());

    src_interpreter agent(ind);
    const any res(agent.run(e));

    const size_t ns(n_slots());
    const size_t last_slot(ns - 1);

    if (res.empty())
      return last_slot;

    const double val(interpreter::to_double(res));
    const size_t where(static_cast<size_t>(normalize_01(val) * ns));

    return (where >= ns) ? last_slot : where;
  }

  ///
  /// \param[in] ind individual used for classification.
  /// \param[out] slot_matrix the main matrix of the dynamic slot algorithm.
  /// \param[out] slot_class slot_class[i] = "label of the predominant class"
  ///                        for the i-th slot.
  ///
  void dyn_slot_evaluator::fill_slots(const individual &ind,
                                      std::vector<uvect> *slot_matrix,
                                      uvect *slot_class,
                                      size_t *dataset_size)
  {
    const data::dataset_t backup(dat_->dataset());
    dat_->dataset(data::training);

    assert(ind.check());
    assert(slot_matrix);
    assert(slot_class);

    std::vector<uvect> &sm(*slot_matrix);  // just for convenience
    const size_t ns(n_slots());
    assert(sm.size() == ns);

    for (size_t i(0); i < ns; ++i)
    {
      assert(sm[i].size() == dat_->classes());

      for (size_t j(0); j < sm[i].size(); ++j)
        sm[i][j] = 0;
    }

    assert(ind.check());

    src_interpreter agent(ind);

    // In the first step this method evaluates the program to obtain an output
    // value for each training example. Based on the program output value a
    // a bidimentional array is built (slots[slot][class]).
    size_t ds_size(0);
    for (data::const_iterator t(dat_->cbegin()); t != dat_->end(); ++t)
    {
      ++ds_size;

      const size_t where(slot(ind, *t));

      ++sm[where][t->label()];
    }

    const size_t unknown(dat_->classes());

    // In the second step the method dynamically determine to which class each
    // slot belongs by simply taking the class with the largest value at the
    // slot...
    for (size_t i(0); i < ns; ++i)
    {
      size_t best_class(0);

      for (size_t j(1); j < sm[i].size(); ++j)
        if (sm[i][j] >= sm[i][best_class])
          best_class = j;

      (*slot_class)[i] = sm[i][best_class] ? best_class : unknown;
    }

    if (dataset_size)
      *dataset_size = ds_size;

    dat_->dataset(backup);
  }

  ///
  /// \param[in] ind program used for class recognition.
  /// \return the fitness (greater is better, max is 0) and the accuracy
  ///         (percentage).
  ///
  /// Slotted Dynamic Class Boundary Determination
  ///
  score_t dyn_slot_evaluator::operator()(const individual &ind)
  {
    assert(ind.check());
    assert(dat_->classes() >= 2);

    const size_t ns(n_slots());
    std::vector<uvect> slot_matrix(ns, uvect(dat_->classes()));
    uvect slot_class(ns);
    size_t count(0);
    fill_slots(ind, &slot_matrix, &slot_class, &count);

    assert(count);

    fitness_t err(0.0);
    for (size_t i(0); i < ns; ++i)
      for (size_t j(0); j < slot_matrix[i].size(); ++j)
        if (j != slot_class[i])
          err += slot_matrix[i][j];

    return score_t(fitness_t(-err),
                   static_cast<double>(count-err) / static_cast<double>(count));
  }

  ///
  /// \param[in] ind individual used for classification.
  /// \param[in] eva evaluator for \a ind.
  ///
  dyn_slot_classifier::dyn_slot_classifier(const individual &ind,
                                           dyn_slot_evaluator *eva)
    : classifier(ind), eva_(eva), slot_name_(eva->n_slots())
  {
    assert(ind.check());
    assert(eva);

    const size_t ns(eva_->n_slots());
    typedef std::vector<unsigned> uvect;

    std::vector<uvect> slot_matrix(ns, uvect(eva_->dat_->classes()));
    uvect slot_class(ns);
    eva_->fill_slots(ind, &slot_matrix, &slot_class);

    assert(slot_name_.size() == slot_class.size());
    for (size_t i(0); i < ns; ++i)
      slot_name_[i] = eva_->dat_->class_name(slot_class[i]);
  }

  ///
  /// \param[in] instance data to be classified.
  /// \return the class that include the \a instance.
  ///
  std::string dyn_slot_classifier::operator()(
    const data::example &instance) const
  {
    const size_t where(eva_->slot(ind_, instance));

    return slot_name_[where];
  }

  ///
  /// \param[in] ind individual used for classification.
  /// \return a vector containing the distribution parameters for each class
  ///         of training set.
  ///
  std::vector<distribution<double>> gaussian_evaluator::gaussian_distribution(
    const individual &ind)
  {
    const data::dataset_t backup(dat_->dataset());
    dat_->dataset(data::training);

    assert(dat_->classes() > 1);
    std::vector<distribution<double>> gauss(dat_->classes());

    assert(ind.check());
    src_interpreter agent(ind);

    // For a set of training data, we assume that the behaviour of a program
    // classifier is modelled using multiple Gaussian distributions, each of
    // which corresponds to a particular class. The distribution of a class is
    // determined by evaluating the program on the examples of the class in
    // the training set. This is done by taking the mean and standard deviation
    // of the program outputs for those training examples for that class.
    for (data::const_iterator t(dat_->cbegin()); t != dat_->cend(); ++t)
    {
      const any res(agent.run(*t));

      double val(res.empty() ? 0.0 : interpreter::to_double(res));
      const double cut(10000000);
      if (val > cut)
        val = cut;
      else if (val < -cut)
        val = -cut;

      gauss[t->label()].add(val);
    }

    dat_->dataset(backup);
    return gauss;
  }

  ///
  /// \param[in] ind program used for class recognition.
  /// \return the fitness (greater is better, max is 0).
  ///
  /// For details about this algorithm see:
  /// * "Using Gaussian Distribution to Construct Fitnesss Functions in Genetic
  ///   Programming for Multiclass Object Classification" - Mengjie Zhang, Will
  ///   Smart (december 2005).
  ///
  score_t gaussian_evaluator::operator()(const individual &ind)
  {
    std::vector<distribution<double>> gauss(gaussian_distribution(ind));

    /*
    fitness_t d(0.0);
    for (size_t i(0); i < gauss.size(); ++i)
      for (size_t j(i+1); j < gauss.size(); ++j)
      {
        const double mean_i(gauss[i].mean);
        const double mean_j(gauss[j].mean);
        const double stddev_i(gauss[i].standard_deviation());
        const double stddev_j(gauss[j].standard_deviation());

        const double delta(std::fabs(mean_j - mean_i));
        const double radius(stddev_j + stddev_i);

        d += 200.0 * std::log(delta) - radius;
      }

    unsigned ok(0), count(0);
    for (auto t(dat_->cbegin()); t != dat_->cend(); ++count, ++t)
      if (class_label(ind, *t, gauss) == t->label())
        ++ok;

    assert(count);

    d = double(ok) - double(count);
    return score_t(d, static_cast<double>(ok) / static_cast<double>(count));
    */

    assert(dat_->classes() > 1);

    fitness_t d(0.0);
    unsigned ok(0), count(0);
    for (auto t(dat_->cbegin()); t != dat_->cend(); ++count, ++t)
    {
      double max_val, sum;
      const unsigned probable_class(class_label(ind, *t, gauss, &max_val,
                                                &sum));

      if (probable_class == t->label())
      {
        ++ok;

        // Note:
        // * (sum - max_val) is the sum of the errors;
        // * (max_val - sum) is the opposite (we want a standardized fitness);
        // * (max_val - sum) / (dat_->classes() - 1) is the opposite of the
        //   average error;
        // * (max_val - 1.0) is the uncertainty about the right class;
        // * 0.001 is a scaling factor.
        d += (max_val - sum) / (dat_->classes() - 1) + 0.001 * (max_val - 1.0);
      }
      else
      {
        // Note:
        // * the maximum single class error is -1.0;
        // * the maximum average class error is -1.0 / dat_->classes();
        // So -1.0 is like to say that we have a complete failure.
        d -= 1.0;
      }
    }

    return score_t(d, static_cast<double>(ok) / static_cast<double>(count));
  }

  ///
  /// \param[in] ind program used for classification.
  /// \param[in] example input value whose class we are interested in.
  /// \param[in] gauss parameters of the gaussian distributions. We have one
  ///                  gaussian distribution for every class.
  /// \param[out] prob
  /// \param[out] prob_sum
  /// \return the class of \a instance.
  ///
  unsigned gaussian_evaluator::class_label(
    const individual &ind,
    const data::example &example,
    const std::vector<distribution<double>> &gauss,
    double *prob, double *prob_sum)
  {
    const any res(src_interpreter(ind).run(example));
    const double x(res.empty() ? 0.0 : interpreter::to_double(res));

    assert(dat_->classes() == gauss.size());

    *prob = *prob_sum = 0.0;
    unsigned probable_class(0);

    for (size_t i(0); i < dat_->classes(); ++i)
    {
      const double distance(std::abs(x - gauss[i].mean));
      const double variance(gauss[i].variance);

      double p(0.0);
      if (variance == 0.0)     // These are borderline cases
        if (distance == 0.0)   // These are borderline cases
          p = 1.0;
        else
          p = 0.0;
      else                     // This is the standard case
        p = std::exp(-0.5 * distance * distance / variance);

      if (p > *prob)
      {
        *prob = p;
        probable_class = i;
      }

      *prob_sum += p;
    }

    return probable_class;
  }

  ///
  /// \param[in] ind individual used for classification.
  /// \param[in] eva evaluator for \a ind.
  ///
  gaussian_classifier::gaussian_classifier(const individual &ind,
                                           gaussian_evaluator *eva)
    : classifier(ind), eva_(eva)
  {
    assert(ind.check());
    assert(eva);

    gauss_ = eva_->gaussian_distribution(ind);
  }

  ///
  ///
  /// \param[in] example data to be classified.
  /// \return the class that include the \a instance.
  ///
  std::string gaussian_classifier::operator()(
    const data::example &example) const
  {
    double dummy;
    return eva_->dat_->class_name(eva_->class_label(ind_, example, gauss_,
                                                    &dummy, &dummy));
  }

  /*
  ///
  /// \param[in] ind an individual.
  /// \param[in] label
  /// \return the fitness of individual \a ind (greater is better, max is 0).
  ///
  template<class T>
  fitness_t problem::binary_fitness(const vita::individual &ind,
                                    unsigned label) const
  {
    assert(dat_.classes() == 2);

    src_interpreter agent(ind);

    fitness_t err(0.0);
    int illegals(0);

    for (data::const_iterator t(dat.begin()); t != dat.end(); ++t)
    {
      for (size_t i(0); i < vars_.size(); ++i)
        vars_[i]->val = t->input[i];

      const any res(agent.run());

      if (res.empty())
        err += std::pow(100.0,++illegals);
      else
      {
        const T val(any_cast<T>(res));
        if ((t->label() == label && val < 0.0) ||
            (t->label() != label && val >= 0.0))
          err += std::fabs(val);
      }
    }

    return -err;
  }
  */
}  // namespace vita

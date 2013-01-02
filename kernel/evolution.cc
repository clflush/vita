/**
 *
 *  \file evolution.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

#include "evolution.h"
#include "environment.h"
#include "random.h"
#include "timer.h"

namespace vita
{
  ///
  /// \param[in] env environment (mostly used for population initialization).
  /// \param[in] eva evaluator used during the evolution.
  /// \param[in] sc function used to identify a stop condition (i.e. it's
  ///               most improbable that evolution will discover better
  ///               solutions).
  /// \param[in] sd the "shake data" function. It is used to alter the training
  ///               set so that evolution would take place in a dynamic
  ///               environment.
  ///
  evolution::evolution(const environment &env, evaluator *const eva,
                       std::function<bool (const summary &)> sc,
                       std::function<void (unsigned)> sd)
    : selection(std::make_shared<tournament_selection>(this)),
      operation(std::make_shared<standard_op>(this, &stats_)),
      replacement(this), pop_(env),
      eva_(new evaluator_proxy(eva, env.ttable_size)), stop_condition_(sc),
      shake_data_(sd)
  {
    assert(eva);

    // When we have a stop_condition function we use it; otherwise the stop
    // criterion is the number of generations.
    if (!stop_condition_)
      stop_condition_ = [this](const summary &s) -> bool
                        {
                          return *pop_.env().g_since_start > 0 &&
                                 s.gen > *pop_.env().g_since_start;
                        };

    assert(check());
  }

  ///
  /// \param[in] elapsed_milli time, in milliseconds, elapsed from the start
  ///                          of evolution.
  /// \return speed of execution (cycles / s).
  ///
  double evolution::get_speed(double elapsed_milli) const
  {
    double speed(0.0);
    if (stats_.gen && elapsed_milli > 0)
      speed = 1000.0 * (pop_.size() * stats_.gen) / elapsed_milli;

    return speed;
  }

  ///
  /// \return statistical informations about the elements of the population.
  ///
  analyzer evolution::get_stats() const
  {
    analyzer az;
    for (unsigned i(0); i < pop_.size(); ++i)
      az.add(pop_[i], fitness(pop_[i]));

    return az;
  }

  ///
  /// \param[out] probes number of probes in the transposition table.
  /// \param[out] hits number of hits in the transposition table (hits <=
  ///             probes).
  ///
  void evolution::get_probes(std::uintmax_t *probes,
                             std::uintmax_t *hits) const
  {
    *probes = *hits = 0;

    if (typeid(*eva_) == typeid(evaluator_proxy))
    {
      *probes = static_cast<evaluator_proxy *>(eva_.get())->probes();
      *hits   = static_cast<evaluator_proxy *>(eva_.get())->hits();
    }
  }

  ///
  /// \param[in] run_count run number.
  ///
  /// Saves working / statistical informations in a log file.
  /// Data are written in a CSV-like fashion and are partitioned in blocks
  /// separated by two blank lines:
  /// [BLOCK_1]\n\n
  /// [BLOCK_2]\n\n
  /// ...
  /// [BLOCK_x]
  /// where each block is a set of line like this:
  /// data_1 [space] data_2 [space] ... [space] data_n
  /// We use this format, instead of XML, because statistics are produced
  /// incrementally and so it's simple and fast to append new data to a
  /// CSV-like file. Note also that data sets are ready to be plotted by
  /// GNUPlot.
  ///
  void evolution::log(unsigned run_count) const
  {
    static unsigned last_run(0);

    if (pop_.env().stat_dynamic)
    {
      const std::string f_dynamic(pop_.env().stat_dir + "/dynamic");
      std::ofstream dynamic(f_dynamic.c_str(), std::ios_base::app);
      if (dynamic.good())
      {
        if (last_run != run_count)
        {
          dynamic << std::endl << std::endl;
          last_run = run_count;
        }

        std::uintmax_t hits(0), probes(0);
        get_probes(&probes, &hits);

        dynamic << run_count
                << ' ' << stats_.gen;

        if (stats_.best)
          dynamic << ' ' << stats_.best->score.fitness
                  << ' ' << stats_.best->score.accuracy;
        else
          dynamic << " ? ?";

        dynamic << ' ' << stats_.az.fit_dist().mean
                << ' ' << stats_.az.fit_dist().standard_deviation()
                << ' ' << stats_.az.fit_dist().entropy()
                << ' ' << stats_.az.fit_dist().min
                << ' ' << unsigned(stats_.az.length_dist().mean)
                << ' ' << stats_.az.length_dist().standard_deviation()
                << ' ' << unsigned(stats_.az.length_dist().max)
                << ' ' << stats_.mutations
                << ' ' << stats_.crossovers
                << ' ' << stats_.az.functions(0)
                << ' ' << stats_.az.terminals(0)
                << ' ' << stats_.az.functions(1)
                << ' ' << stats_.az.terminals(1)
                << ' ' << hits
                << ' ' << probes;

        for (unsigned active(0); active <= 1; ++active)
          for (analyzer::const_iterator i(stats_.az.begin());
               i != stats_.az.end();
               ++i)
            dynamic << ' ' << (i->first)->display() << ' '
                    << i->second.counter[active];

        dynamic << ' ' << '"';
        if (stats_.best)
          stats_.best->ind.in_line(dynamic);
        dynamic << '"' << std::endl;
      }
    }
  }

  ///
  /// \param[in] ind individual whose accuracy/fitness we are interested in.
  /// \return the fitness and the accuracy of \a ind.
  ///
  score_t evolution::score(const individual &ind) const
  {
    return (*eva_)(ind);
  }

  ///
  /// \param[in] ind individual whose fitness we are interested in.
  /// \return the fitness of \a ind.
  ///
  fitness_t evolution::fitness(const individual &ind) const
  {
    return score(ind).fitness;
  }

  ///
  /// \param[in] ind individual whose fitness we are interested in.
  /// \return the fitness of \a ind.
  ///
  fitness_t evolution::fast_fitness(const individual &ind) const
  {
    return eva_->fast(ind).fitness;
  }

  ///
  /// \param[in] verbose if \c true prints verbose informations.
  /// \param[in] run_count run number (used for print and log).
  /// \param[in] rep_id index of the active replacement strategy.
  ///
  /// The genetic programming loop:
  /// * select the individual(s) to participate (default algorithm: tournament
  ///   selection) in the genetic operation;
  /// * perform genetic operation creating a new offspring individual;
  /// * place the offspring into the original population (steady state)
  ///   replacing a bad individual.
  /// This whole process repeats until the termination criteria is satisfied.
  /// With any luck, it will produce an individual that solves the problem at
  /// hand.
  ///
  const summary &evolution::run(bool verbose, unsigned run_count,
                                replacement_factory::strategy rep_id)
  {
    stats_.clear();
    stats_.best = {pop_[0], score(pop_[0])};

    eva_->clear();

    timer measure;

    for (stats_.gen = 0; !stop_condition_(stats_); ++stats_.gen)
    {
      if (shake_data_)
      {
        shake_data_(stats_.gen);

        // If we 'shake' the data, the statistics picked so far have to be
        // cleared (the best individual and its fitness refer to an old
        // training set).
        stats_.best = {pop_[0], score(pop_[0])};
      }

      stats_.az = get_stats();
      log(run_count);

      for (unsigned k(0); k < pop_.size(); ++k)
      {
        if (verbose && k % std::max(pop_.size()/100, static_cast<size_t>(2)))
          std::cout << "Run " << run_count << '.' << stats_.gen << " ("
                    << std::setw(3) << 100 * k / pop_.size() << "%)\r"
                    << std::flush;

        // --------- SELECTION ---------
        std::vector<index_t> parents(selection->run());

        // --------- CROSSOVER / MUTATION ---------
        std::vector<individual> off(operation->run(parents));

        // --------- REPLACEMENT --------
        const fitness_t before(stats_.best->score.fitness);
        replacement[rep_id](parents, off, &stats_);

        if (verbose && stats_.best->score.fitness != before)
        {
          std::cout << "Run " << run_count << '.' << std::setw(6)
                    << stats_.gen << " (" << std::setw(3)
                    << 100 * k / pop_.size() << "%): fitness "
                    << std::setw(16) << stats_.best->score.fitness;

          if (stats_.best->score.accuracy >= 0.0)
            std::cout << std::setprecision(2) << " (" << std::fixed
                      << std::setw(6) << 100.0 * stats_.best->score.accuracy
                      << "%)" << std::setprecision(-1)
                      << std::resetiosflags(std::ios::fixed);

          std::cout << std::endl;
        }
      }

      stats_.speed = get_speed(measure.elapsed());
      get_probes(&stats_.ttable_probes, &stats_.ttable_hits);
    }

    if (verbose)
    {
      double speed(stats_.speed);
      std::string unit;

      if (speed >= 1.0)
        unit = "cycles/s";
      else if (speed >= 0.1)
      {
        speed *= 3600.0;
        unit = "cycles/h";
      }
      else  // speed < 0.1
      {
        speed *= 3600.0*24.0;
        unit = "cycles/day";
      }

      std::cout << static_cast<unsigned>(speed) << unit << std::string(10, ' ')
                << std::endl << std::string(40, '-') << std::endl;
    }

    return stats_;
  }

  ///
  /// \return true if the object passes the internal consistency check.
  ///
  bool evolution::check() const
  {
    return pop_.check() && eva_ && stop_condition_;
  }

  ///
  /// Default constructor just call the summary::clear method.
  ///
  summary::summary()
  {
    clear();
  }

  ///
  /// Resets summary informations.
  ///
  void summary::clear()
  {
    ttable_probes = 0;
    ttable_hits   = 0;
    mutations     = 0;
    crossovers    = 0;
    gen           = 0;
    testset       = 0;
    last_imp      = 0;
    speed         = 0.0;

    az.clear();

    best = boost::none;
  }
}  // namespace vita

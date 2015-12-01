/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2013-2015 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#if !defined(VITA_EVOLUTION_H)
#  error "Don't include this file directly, include the specific .h instead"
#endif

#if !defined(VITA_EVOLUTION_TCC)
#define      VITA_EVOLUTION_TCC

namespace term
{
///
/// \return `true` when the user press the '.' key.
///
inline bool user_stop()
{
  const bool stop(kbhit() && std::cin.get() == '.');

  if (stop)
    print.info("Stopping evolution...");

  return stop;
}

///
/// Resets the term and restores the default signal handlers.
///
inline void reset()
{
  std::signal(SIGABRT, SIG_DFL);
  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);

  term_raw_mode(false);
}

///
/// If the program receives a SIGABRT / SIGINT / SIGTERM, it must handle
/// the signal and reset the terminal to the initial state.
///
inline void signal_handler(int signum)
{
  term::reset();

  std::raise(signum);
}

///
/// Sets the term in raw mode and handles the interrupt signals.
///
inline void set()
{
  // Install our signal handler.
  std::signal(SIGABRT, term::signal_handler);
  std::signal(SIGINT, term::signal_handler);
  std::signal(SIGTERM, term::signal_handler);

  term_raw_mode(true);
}
}  // namespace term

///
/// \param[in] e environment (mostly used for population initialization).
/// \param[in] eva evaluator used during the evolution.
/// \param[in] sc function used to identify a stop condition (i.e. it's
///               most improbable that evolution will discover better
///               solutions).
/// \param[in] sd the "shake data" function. It is used to alter the training
///               set so that evolution would take place in a dynamic
///               environment.
///
template<class T, template<class> class ES>
evolution<T, ES>::evolution(const environment &e, evaluator<T> &eva,
                            std::function<bool (const summary<T> &)> sc,
                            std::function<void (unsigned)> sd)
  : pop_(e), eva_(eva), es_(pop_, eva, &stats_),
    external_stop_condition_(sc), shake_data_(sd)
{
  assert(e.sset);
  assert(debug());
}

///
/// \param[in] s an up to date evolution summary.
/// \return `true` when evolution should be interrupted.
///
template<class T, template<class> class ES>
bool evolution<T, ES>::stop_condition(const summary<T> &s) const
{
  assert(env().generations);

  // Check the number of generations.
  if (s.gen > env().generations)
    return true;

  if (term::user_stop())
    return true;

  // When we have an external_stop_condition_ function we use it.
  if (external_stop_condition_)
    return external_stop_condition_(s);

  return false;
}

///
/// \return statistical informations about the elements of the population.
///
template<class T, template<class> class ES>
analyzer<T> evolution<T, ES>::get_stats() const
{
  analyzer<T> az;

  for (auto it(pop_.begin()), end(pop_.end()); it != end; ++it)
    az.add(*it, eva_(*it), it.layer());

  return az;
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
///   data_1 [space] data_2 [space] ... [space] data_n
/// We use this format, instead of XML, because statistics are produced
/// incrementally and so it's simple and fast to append new data to a
/// CSV-like file. Note also that data sets are ready to be plotted by
/// GNUPlot.
///
template<class T, template<class> class ES>
void evolution<T, ES>::log(unsigned run_count) const
{
  static unsigned last_run(0);

  auto fullpath = [this](const std::string &f)
                  {
                    return env().stat.dir + "/" + f;
                  };

  if (env().stat.dynamic)
  {
    std::ofstream f_dyn(fullpath(env().stat.dyn_name), std::ios_base::app);
    if (f_dyn.good())
    {
      if (last_run != run_count)
        f_dyn << "\n\n";

      f_dyn << run_count << ' ' << stats_.gen;

      if (stats_.best.solution.empty())
        f_dyn << " ?";
      else
        f_dyn << ' ' << stats_.best.score.fitness[0];

      f_dyn << ' ' << stats_.az.fit_dist().mean()[0]
            << ' ' << stats_.az.fit_dist().standard_deviation()[0]
            << ' ' << stats_.az.fit_dist().entropy()
            << ' ' << stats_.az.fit_dist().min()[0]
            << ' ' << static_cast<unsigned>(stats_.az.length_dist().mean())
            << ' ' << stats_.az.length_dist().standard_deviation()
            << ' ' << static_cast<unsigned>(stats_.az.length_dist().max())
            << ' ' << stats_.mutations
            << ' ' << stats_.crossovers
            << ' ' << stats_.az.functions(0)
            << ' ' << stats_.az.terminals(0)
            << ' ' << stats_.az.functions(1)
            << ' ' << stats_.az.terminals(1);

      for (unsigned active(0); active <= 1; ++active)
        for (const auto &symb_stat : stats_.az)
          f_dyn << ' ' << (symb_stat.first)->display()
                << ' ' << symb_stat.second.counter[active];

      f_dyn << " \"";
      if (!stats_.best.solution.empty())
        stats_.best.solution.in_line(f_dyn);
      f_dyn << "\"\n";
    }
  }

  if (env().stat.population)
  {
    std::ofstream f_pop(fullpath(env().stat.pop_name), std::ios_base::app);
    if (f_pop.good())
    {
      if (last_run != run_count)
        f_pop << "\n\n";

      for (const auto &f : stats_.az.fit_dist().seen())
        // f.first: value, f.second: frequency
        f_pop << run_count << ' ' << stats_.gen << ' '
              << std::fixed << std::scientific
              << std::setprecision(
                   std::numeric_limits<fitness_t::value_type>::digits10 + 2)
              << f.first[0] << ' ' << f.second << '\n';
    }
  }

  es_.log(last_run, run_count);

  if (last_run != run_count)
    last_run = run_count;
}

///
/// \param[in] k current generation.
/// \param[in] run_count total number of runs planned.
/// \param[in] status if `true` print a run/generation/fitness status line.
///
/// Print evolution informations (if `environment::log_level >= OUTPUT`).
///
template<class T, template<class> class ES>
void evolution<T, ES>::print_progress(unsigned k, unsigned run_count,
                                      bool status) const
{
  if (print.verbosity() >= log::L_OUTPUT)
  {
    const unsigned perc(100 * k / pop_.individuals());
    if (status)
      std::cout << "Run " << run_count << '.' << std::setw(6)
                << stats_.gen << " (" << std::setw(3)
                << perc << "%): fitness " << stats_.best.score.fitness
                << '\n';
    else
      std::cout << "Crunching " << run_count << '.' << stats_.gen << " ("
                << std::setw(3) << perc << "%)\r" << std::flush;
  }
}

///
/// \param[in] run_count run number (used for printing and logging).
/// \return a partial summary of the search (see notes).
///
/// The genetic programming loop:
///
/// * select the individual(s) to participate (default algorithm: tournament
///   selection) in the genetic operation;
/// * perform genetic operation creating a new offspring individual;
/// * place the offspring into the original population (steady state)
///   replacing a bad individual.
///
/// This whole process repeats until the termination criteria is satisfied.
/// With any luck, it will produce an individual that solves the problem at
/// hand.
///
/// \note
/// The return value is a partial summary: the `measurement` section is only
/// partially filled (fitness) since many metrics are expensive to calculate
/// and not significative for all kind of problems (e.g. f1-score for a
/// symbolic regression problem). The src_search class has a simple scheme to
/// request the computation of additional metrics.
///
template<class T, template<class> class ES>
const summary<T> &
evolution<T, ES>::run(unsigned run_count)
{
  stats_.clear();
  stats_.best.solution = pop_[{0, 0}];
  stats_.best.score.fitness = eva_(stats_.best.solution);

  timer measure;

  bool stop(false);
  term::set();

#if defined(CLONE_SCALING)
  eva_.clear(evaluator<T>::stats);
#endif

  es_.pre_bookkeeping();  // hook for strategy-specific preliminary bookkeeping

  for (stats_.gen = 0; !stop_condition(stats_) && !stop;  ++stats_.gen)
  {
    if (shake_data_ && stats_.gen % 4 == 0)
    {
      shake_data_(stats_.gen);

      // If we 'shake' the data, the statistics picked so far have to be
      // cleared (the best individual and its fitness refer to an old
      // training set).
      assert(!stats_.best.solution.empty());
      stats_.best.score.fitness = eva_(stats_.best.solution);
      print_progress(0, run_count, true);
    }

    stats_.az = get_stats();
    log(run_count);

    timer from_last_msg;
    for (unsigned k(0); k < pop_.individuals() && !stop; ++k)
    {
      if (from_last_msg.elapsed() > 1000.0)
      {
        print_progress(k, run_count, false);

        from_last_msg.restart();
        stop = term::user_stop();
      }

      // --------- SELECTION ---------
      auto parents(es_.selection.run());

      // --------- CROSSOVER / MUTATION ---------
      auto off(es_.recombination.run(parents));

      // --------- REPLACEMENT --------
      const auto before(stats_.best.score.fitness);
      es_.replacement.run(parents, off, &stats_);

      if (stats_.best.score.fitness != before)
        print_progress(k, run_count, true);
    }

    stats_.elapsed = measure.elapsed();

    es_.post_bookkeeping();  // hook for strategy-specific final bookkeeping
  }

  print.info("Elapsed time: ", stats_.elapsed / 1000.0, "s",
             std::string(10, ' '));

  term::reset();
  return stats_;
}

///
/// \param[in] verbose if `true` prints error messages to `std::cerr`.
/// \return true if object passes the internal consistency check.
///
template<class T, template<class> class ES>
bool evolution<T, ES>::debug() const
{
  return pop_.debug();
}
#endif  // Include guard

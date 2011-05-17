/**
 *
 *  \file environment.h
 *
 *  \author Manlio Morini
 *  \date 2011/05/11
 *
 *  This file is part of VITA
 *
 */
  
#if !defined(ENVIRONMENT_H)
#define      ENVIRONMENT_H

#include <cmath>
#include <string>
#include <boost/property_tree/ptree.hpp>

#include "vita.h"
#include "symbol_set.h"

namespace vita
{

  class environment
  {
  public:
    environment();

    void log(boost::property_tree::ptree &, const std::string & = "") const;

    void insert(symbol *const);

    /// The number of genes (maximum length of an evolved program in the 
    /// population).
    unsigned code_length;

    /// An elitist algorithm is one that ALWAYS retains in the population the
    /// best individual found so far. With higher elitism the population will
    /// converge quicker but losing diversity. 
    bool elitism;

    /// Mutation probability. Mutation is one of the principal "search 
    /// operators" used to transform programs in the Genetic Programming
    /// algorithm. Mutation causes random changes in individuals.
    double p_mutation;
    /// Crossover probability.
    double p_cross;

    /// Population's size (number of programs/individuals in the population).
    unsigned long individuals;

    /// Size of the tournament to choose the parents.
    /// Tournament sizes tend to be small relative to the population size. The
    /// ratio of tournament size to population size can be used as a measure of
    /// selective pressure. Note that a tournament size of 1 would be 
    /// equivalent to selecting individuals at random (within the mate zone).
    unsigned par_tournament;
    /// Size of the tournament to choose replaced individuals.
    /// Tournament sizes tend to be small relative to the population size. The
    /// ratio of tournament size to population size can be used as a measure of
    /// selective pressure. Note that a tournament size of 1 would be 
    /// equivalent to selecting individuals at random (within the mate zone).
    unsigned rep_tournament;
    /// Mating zone. 0 for panmictic.
    unsigned mate_zone;

    /// Maximun number of generations allowed before terminate a run (0 means
    /// no limit).
    unsigned g_since_start;
    /// Stop a run when we cannot see improvements within g_without_improvement
    /// generations.
    unsigned g_without_improvement;

    /// Should we use Adaptive Representation through Learning?
    bool arl;

    /// Should we always insert input variables at the end of individual?
    bool input;

    /// 2^ttable_size is the number of elements of the transposition table.
    unsigned ttable_size;

    /// Where shuld we save statistics / status files?
    std::string stat_dir;

    /// Save a list of active ADF?
    bool stat_arl;

    /// Should we save a dynamic execution status file?
    bool stat_dynamic;

    /// Should we save a summary of the run?
    bool stat_summary;

    symbol_set sset;

    bool check() const;

    static const char arl_filename[];
    static const char dyn_filename[];
    static const char sum_filename[];
  };

}  // namespace vita

#endif  // ENVIRONMENT_H

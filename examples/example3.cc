/**
 *
 *  \file example3.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011, 2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include <cstdlib>
#include <iostream>

#include "environment.h"
#include "individual.h"
#include "primitive/factory.h"

//
// Performs three types of crossover between two random individuals.
//
int main(int argc, char *argv[])
{
  vita::environment env(true);

  env.code_length = argc > 1 ? atoi(argv[1]) : 10;

  vita::symbol_set sset;

  vita::symbol_factory &factory(vita::symbol_factory::instance());
  sset.insert(factory.make(vita::d_double, -200, 200));
  sset.insert(factory.make("FADD"));
  sset.insert(factory.make("FSUB"));
  sset.insert(factory.make("FMUL"));
  sset.insert(factory.make("FIFL"));
  sset.insert(factory.make("FIFE"));

  vita::individual i1(env, sset), i2(env, sset);

  std::cout << "PARENTS" << std::endl << std::string(40, '-') << std::endl;
  i1.dump(std::cout);
  std::cout << std::endl;
  i2.dump(std::cout);
  std::cout << std::endl;

  std::cout << "OFFSPRING (UNIFORM CROSSOVER)" << std::endl
            << std::string(40, '-') << std::endl;

  uniform_crossover(i1, i2).dump(std::cout);
  std::cout << std::endl;

  std::cout << "OFFSPRING (ONE POINT CROSSOVER)" << std::endl
            << std::string(40, '-') << std::endl;

  one_point_crossover(i1, i2).dump(std::cout);
  std::cout << std::endl;

  std::cout << "OFFSPRING (TWO POINTS CROSSOVER)" << std::endl
            << std::string(40, '-') << std::endl;

  two_point_crossover(i1, i2).dump(std::cout);
  return EXIT_SUCCESS;
}

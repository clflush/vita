/**
 *
 *  \file example6.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2011-2013 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <memory>

#include "kernel/environment.h"
#include "kernel/evolution.h"
#include "kernel/interpreter.h"
#include "kernel/terminal.h"
#include "kernel/src/primitive/factory.h"

// This class models the first input.
class X : public vita::terminal
{
public:
  X() : vita::terminal("X", 0, true) {}

  vita::any eval(vita::interpreter *) const { return vita::any(val); }

  static double val;
};

class Y : public vita::terminal
{
public:
  Y() : vita::terminal("Y", 0, true) {}

  vita::any eval(vita::interpreter *) const { return vita::any(val); }

  static double val;
};

class Z : public vita::terminal
{
public:
  Z() : vita::terminal("Z", 0, true) {}

  vita::any eval(vita::interpreter *) const { return vita::any(val); }

  static double val;
};

double X::val;
double Y::val;
double Z::val;

class my_evaluator : public vita::evaluator
{
  vita::fitness_t operator()(const vita::individual &ind)
  {
    vita::interpreter agent(ind);

    vita::fitness_t::base_t fit(0.0);
    for (double x(0); x < 10; ++x)
      for (double y(0); y < 10; ++y)
        for (double z(0); z < 10; ++z)
        {
          X::val = x;
          Y::val = y;
          Z::val = z;

          const vita::any res(agent.run());

          if (!res.empty())
          {
            const auto dres(vita::any_cast<double>(res));
            assert(std::isfinite(dres));
            fit += std::exp(-std::fabs(dres - (x*x + y*y - z*z)));
          }
        }

    return vita::fitness_t(fit);
  }
};

int main(int argc, char *argv[])
{
  vita::environment env(true);

  env.individuals = argc > 1 ? atoi(argv[1]) : 100;
  env.code_length = argc > 2 ? atoi(argv[2]) : 100;
  env.g_since_start = argc > 3 ? atoi(argv[3]) : 100;

  vita::symbol_set sset;

  vita::symbol_factory &factory(vita::symbol_factory::instance());
  sset.insert(vita::make_unique<X>());
  sset.insert(vita::make_unique<Y>());
  sset.insert(vita::make_unique<Z>());
  sset.insert(factory.make("FADD"));
  sset.insert(factory.make("FSUB"));
  sset.insert(factory.make("FMUL"));
  sset.insert(factory.make("FIFL"));
  sset.insert(factory.make("FIFE"));

  std::unique_ptr<vita::evaluator> eva(vita::make_unique<my_evaluator>());

  vita::evolution<vita::individual> evo(env, sset, eva.get());

  evo.run<vita::std_es>(1);

  return EXIT_SUCCESS;
}

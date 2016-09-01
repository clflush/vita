/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2011-2016 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#include <iostream>
#include <fstream>

#include "kernel/vita.h"

using i_interp = vita::core_interpreter;

// This class models the first input.
class X : public vita::terminal
{
public:
  X() : vita::terminal("X", 0) {}

  virtual bool input() const override { return true; }

  virtual vita::any eval(i_interp *) const override { return vita::any(val); }

  static double val;
};

class Y : public vita::terminal
{
public:
  Y() : vita::terminal("Y", 0) {}

  virtual bool input() const override { return true; }

  virtual vita::any eval(i_interp *) const override { return vita::any(val); }

  static double val;
};

class Z : public vita::terminal
{
public:
  Z() : vita::terminal("Z", 0) {}

  virtual bool input() const override { return true; }

  virtual vita::any eval(i_interp *) const override { return vita::any(val); }

  static double val;
};

double X::val;
double Y::val;
double Z::val;

class my_evaluator : public vita::evaluator<vita::i_mep>
{
  virtual vita::fitness_t operator()(const vita::i_mep &ind) override
  {
    vita::interpreter<vita::i_mep> agent(&ind);

    vita::fitness_t::value_type fit(0.0);
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

    return {fit};
  }
};

int main(int argc, char *argv[])
{
  vita::symbol_set sset;
  vita::environment env(&sset, true);

  env.individuals = static_cast<unsigned>(argc > 1 ? std::atoi(argv[1]) : 100);
  env.code_length = static_cast<unsigned>(argc > 2 ? std::atoi(argv[2]) : 100);
  env.generations = static_cast<unsigned>(argc > 3 ? std::atoi(argv[3]) : 100);

  vita::symbol_factory factory;
  sset.insert(std::make_unique<X>());
  sset.insert(std::make_unique<Y>());
  sset.insert(std::make_unique<Z>());
  sset.insert(factory.make("FADD"));
  sset.insert(factory.make("FSUB"));
  sset.insert(factory.make("FMUL"));
  sset.insert(factory.make("FIFL"));
  sset.insert(factory.make("FIFE"));

  auto eva(std::make_unique<my_evaluator>());

  vita::evolution<vita::i_mep, vita::std_es> evo(env, *eva);

  evo.run(1);
}

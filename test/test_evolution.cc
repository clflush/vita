/**
 *
 *  \file test_evolution.cc
 *
 *  Copyright (c) 2011 EOS di Manlio Morini.
 *
 *  This file is part of VITA.
 *
 *  VITA is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  VITA is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with VITA. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstdlib>
#include <iostream>

#include "kernel/environment.h"
#include "kernel/evolution.h"
#include "kernel/primitive/double_pri.h"

#define BOOST_TEST_MODULE Evolution
#include "boost/test/unit_test.hpp"

using namespace boost;

struct F
{
  F()
    : num(new vita::sr::number(-200, 200)),
      f_add(new vita::sr::add()),
      f_sub(new vita::sr::sub()),
      f_mul(new vita::sr::mul()),
      f_ifl(new vita::sr::ifl()),
      f_ife(new vita::sr::ife())
  {
    BOOST_TEST_MESSAGE("Setup fixture");
    env.insert(num);
    env.insert(f_add);
    env.insert(f_sub);
    env.insert(f_mul);
    env.insert(f_ifl);
    env.insert(f_ife);
  }

  ~F()
  {
    BOOST_TEST_MESSAGE("Teardown fixture");
  }

  vita::symbol_ptr num;
  vita::symbol_ptr f_add;
  vita::symbol_ptr f_sub;
  vita::symbol_ptr f_mul;
  vita::symbol_ptr f_ifl;
  vita::symbol_ptr f_ife;

  vita::environment env;
};


BOOST_FIXTURE_TEST_SUITE(Evolution, F)

BOOST_AUTO_TEST_CASE(Creation)
{
  for (unsigned n(4); n <= 100; ++n)
    for (unsigned l(1); l <= 100; l+=(l < 10 ? 1 : 30))
    {
      env.individuals = n;
      env.code_length = l;

      std::auto_ptr<vita::evaluator> eva(new vita::random_evaluator());
      vita::evolution evo(&env, eva.get());

      /*
      if (unit_test::runtime_config::log_level() <= unit_test::log_messages)
      {
        vita::analyzer ay;
        evo.pick_stats(&ay);

        const boost::uint64_t nef(ay.functions(true));
        const boost::uint64_t net(ay.terminals(true));
        const boost::uint64_t ne(nef + net);

        std::cout << std::string(40, '-') << std::endl;
        for (vita::analyzer::const_iterator i(ay.begin());
             i != ay.end();
             ++i)
          std::cout << std::setfill(' ') << (i->first)->display() << ": "
                    << std::setw(5) << i->second.counter[true]
                    << " (" << std::setw(3) << 100*i->second.counter[true]/ne
                    << "%)" << std::endl;

        std::cout << "Average code length: " << ay.length_dist().mean
                  << std::endl
                  << "Code length standard deviation: "
                  << std::sqrt(ay.length_dist().variance) << std::endl
                  << "Max code length: " << ay.length_dist().max << std::endl
                  << "Functions: " << nef << " (" << nef*100/ne << "%)"
                  << std::endl
                  << "Terminals: " << net << " (" << net*100/ne << "%)"
                  << std::endl << std::string(40,'-') << std::endl;
      }
      */

      BOOST_REQUIRE(evo.check());
    }
}

BOOST_AUTO_TEST_SUITE_END()
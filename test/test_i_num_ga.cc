/**
 *  \file
 *  \remark This file is part of VITA.
 *
 *  \copyright Copyright (C) 2014 EOS di Manlio Morini.
 *
 *  \license
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 */

#include <cstdlib>
#include <sstream>

#include "kernel/ga/i_num_ga.h"

#if !defined(MASTER_TEST_SET)
#define BOOST_TEST_MODULE t_i_num_ga
#include <boost/test/unit_test.hpp>

using namespace boost;

constexpr double epsilon(0.00001);

#include "factory_fixture5.h"
#endif

BOOST_FIXTURE_TEST_SUITE(t_i_num_ga, F_FACTORY5)

BOOST_AUTO_TEST_CASE(RandomCreation)
{
  BOOST_TEST_CHECKPOINT("Random creation");
  for (unsigned i(1); i < 1000; ++i)
  {
    vita::i_num_ga ind(env, sset);

    BOOST_REQUIRE(ind.debug());
    BOOST_REQUIRE_EQUAL(ind.size(), sset.categories());
    BOOST_REQUIRE_EQUAL(ind.age(), 0);
  }
}

BOOST_AUTO_TEST_CASE(Mutation)
{
  vita::i_num_ga t(env, sset);
  const vita::i_num_ga orig(t);

  const unsigned n(1000);

  BOOST_TEST_CHECKPOINT("Zero probability mutation");
  env.p_mutation = 0.0;
  for (unsigned i(0); i < n; ++i)
  {
    t.mutation();
    BOOST_REQUIRE_EQUAL(t, orig);
  }

  BOOST_TEST_CHECKPOINT("50% probability mutation.");
  env.p_mutation = 0.5;
  unsigned diff(0);

  for (unsigned i(0); i < n; ++i)
  {
    auto i1(orig);

    i1.mutation();
    diff += orig.distance(i1);
  }

  const double perc(100.0 * double(diff) / double(orig.size() * n));
  BOOST_CHECK_GT(perc, 47.0);
  BOOST_CHECK_LT(perc, 53.0);
}

BOOST_AUTO_TEST_CASE(Comparison)
{
  for (unsigned i(0); i < 2000; ++i)
  {
    vita::i_num_ga a(env, sset);
    BOOST_REQUIRE_EQUAL(a, a);
    BOOST_REQUIRE_EQUAL(a.distance(a), 0);

    vita::i_num_ga b(a);
    BOOST_REQUIRE_EQUAL(a.signature(), b.signature());
    BOOST_REQUIRE_EQUAL(a, b);
    BOOST_REQUIRE_EQUAL(a.distance(b), 0);

    vita::i_num_ga c(env, sset);
    if (a.signature() != c.signature())
    {
      BOOST_REQUIRE_NE(a, c);
      BOOST_REQUIRE_GT(a.distance(c), 0);
      BOOST_REQUIRE_EQUAL(a.distance(c), c.distance(a));
    }
  }
}

BOOST_AUTO_TEST_CASE(Iterators)
{
  for (unsigned j(0); j < 1000; ++j)
  {
    vita::i_num_ga ind(env, sset);

    unsigned i(0);
    for (const auto &g : ind)
    {
      BOOST_CHECK_EQUAL(g, ind[i]);
      ++i;
    }
  }
}

BOOST_AUTO_TEST_CASE(StandardCrossover)
{
  vita::i_num_ga i1(env, sset), i2(env, sset);

  double dist(0.0);
  const unsigned n(1000);
  for (unsigned j(0); j < n; ++j)
  {
    const auto ic(i1.crossover(i2));
    BOOST_CHECK(ic.debug(true));

    dist += i1.distance(ic);
  }

  // +1 since we have at least one gene involved in crossover.
  const double perc(100.0 * dist / ((sset.categories() + 1) * n));
  BOOST_CHECK_GT(perc, 48.0);
  BOOST_CHECK_LT(perc, 52.0);
}

BOOST_AUTO_TEST_CASE(DeCrossover)
{
  for (unsigned j(0); j < 1000; ++j)
  {
    const vita::i_num_ga base(env, sset);
    vita::i_num_ga i1(env, sset), i2(env, sset);

    auto off(base.crossover(i1, i1));

    for (unsigned i(0); i < base.size(); ++i)
      BOOST_CHECK_CLOSE(off(i), base(i), epsilon);

    for (unsigned i(0); i < base.size(); ++i)
    {
      const auto delta(env.de.weight[1] * std::abs(i1(i) - i2(i)));

      if (std::abs(off(i) - base(i)) > epsilon)
      {
        BOOST_CHECK_GT(off(i), base(i) - delta);
        BOOST_CHECK_LT(off(i), base(i) + delta);
      }
      else
        BOOST_CHECK_CLOSE(base(i), off(i), epsilon);
    }
  }
}

BOOST_AUTO_TEST_CASE(Serialization)
{
  for (unsigned i(0); i < 2000; ++i)
  {
    std::stringstream ss;
    vita::i_num_ga i1(env, sset);

    for (auto j(vita::random::between(0u, 100u)); j; --j)
      i1.inc_age();

    BOOST_REQUIRE(i1.save(ss));

    vita::i_num_ga i2(env, sset);
    BOOST_REQUIRE(i2.load(ss));
    BOOST_REQUIRE(i2.debug());

    BOOST_CHECK_EQUAL(i1, i2);
  }
}
BOOST_AUTO_TEST_SUITE_END()
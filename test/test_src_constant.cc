/**
 *
 *  \file test_src_constant.cc
 *  \remark This file is part of VITA.
 *
 *  Copyright (C) 2013-2014 EOS di Manlio Morini.
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this file,
 *  You can obtain one at http://mozilla.org/MPL/2.0/
 *
 */

#include "kernel/src/constant.h"

#if !defined(MASTER_TEST_SET)
#define BOOST_TEST_MODULE src_constant
#include "boost/test/unit_test.hpp"

using namespace boost;
#endif

BOOST_AUTO_TEST_SUITE(src_constant)

BOOST_AUTO_TEST_CASE(Base)
{
  const bool b(true);
  vita::constant<bool> bc(b);
  BOOST_CHECK_EQUAL(vita::any_cast<bool>(bc.eval(nullptr)), b);
  BOOST_CHECK(bc.debug());

  const int i(1234);
  vita::constant<int> ic(i);
  BOOST_CHECK_EQUAL(vita::any_cast<int>(ic.eval(nullptr)), i);
  BOOST_CHECK(ic.debug());

  const double d(3.14);
  vita::constant<double> dc(d);
  BOOST_CHECK_EQUAL(vita::any_cast<double>(dc.eval(nullptr)), d);
  BOOST_CHECK(dc.debug());

  const std::string s("A STRING CONSTANT");
  vita::constant<std::string> sc(s);
  BOOST_CHECK_EQUAL(vita::any_cast<std::string>(sc.eval(nullptr)), s);
  BOOST_CHECK(sc.debug());
}

BOOST_AUTO_TEST_SUITE_END()

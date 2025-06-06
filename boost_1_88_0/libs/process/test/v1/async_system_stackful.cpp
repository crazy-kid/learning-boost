// Copyright (c) 2016 Klemens D. Morgenstern
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>

#include <boost/process/v1/error.hpp>
#include <boost/process/v1/io.hpp>
#include <boost/process/v1/async.hpp>
#include <boost/process/v1/child.hpp>
#include <boost/process/v1/async_system.hpp>

#include <string>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/yield.hpp>

#include <vector>
#include <array>
BOOST_AUTO_TEST_SUITE( async );

namespace bp = boost::process::v1;
BOOST_AUTO_TEST_CASE(stackful, *boost::unit_test::timeout(15))
{
    using boost::unit_test::framework::master_test_suite;

    bool did_something_else = false;

    boost::asio::io_context ios;
    auto stackful =
            [&](boost::asio::yield_context yield_)
            {
                int ret =
                    bp::async_system(
                        ios, yield_,
                        master_test_suite().argv[1],
                        "test", "--wait", "1");

                BOOST_CHECK_EQUAL(ret, 0);
                BOOST_CHECK(did_something_else);
            };

    boost::asio::io_context::strand str{ios};
    boost::asio::post(str, [&]{boost::asio::spawn(ios, stackful, boost::asio::detached);});
    boost::asio::post(str, [&]{did_something_else = true;});

    ios.run();
    BOOST_CHECK(did_something_else);
}

BOOST_AUTO_TEST_SUITE_END();
// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MAIN
#define BOOST_TEST_IGNORE_SIGCHLD
#include <boost/test/included/unit_test.hpp>

#include <boost/process/v1/error.hpp>
#include <boost/process/v1/system.hpp>
#include <boost/process/v1/env.hpp>
#include <boost/process/v1/cmd.hpp>

#include <boost/process/v1/filesystem.hpp>
#include <boost/filesystem/operations.hpp>

#include <system_error>

#include <boost/algorithm/string/case_conv.hpp>

#include <boost/system/error_code.hpp>
#include <cstdlib>

namespace bp = boost::process::v1;
namespace fs = boost::process::v1::filesystem;


BOOST_AUTO_TEST_CASE(explicit_)
{
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;

    fs::path pth = master_test_suite().argv[1];
    auto env = boost::this_process::environment();

    auto itr = std::find_if(env.begin(), env.end(),
            [](const bp::native_environment::entry_type & e){return boost::to_upper_copy(e.get_name()) == "PATH";});

    BOOST_REQUIRE(itr != env.end());

    (*itr) += fs::canonical(fs::absolute(pth.parent_path())).string();

    int ret = bp::system(
        bp::cmd="sparring_partner --exit-code 42",
        ec
    );

    BOOST_CHECK(!ec);
    if (ec)
        BOOST_TEST_MESSAGE(ec.message());
    BOOST_CHECK_EQUAL(ret, 42);
}

BOOST_AUTO_TEST_CASE(implicit)
{
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;

    fs::path pth = master_test_suite().argv[1];
    auto env = boost::this_process::environment();

    auto itr = std::find_if(env.begin(), env.end(),
            [](const bp::native_environment::entry_type & e){return boost::to_upper_copy(e.get_name()) == "PATH";});

    BOOST_REQUIRE(itr != env.end());

    (*itr) += fs::canonical(fs::absolute(pth.parent_path())).string();

    int ret = bp::system(
        "sparring_partner --exit-code 21",
        ec
    );

    BOOST_CHECK(!ec);
    if (ec)
        BOOST_TEST_MESSAGE(ec.message());
    BOOST_CHECK_EQUAL(ret, 21);
}

BOOST_AUTO_TEST_CASE(empty_cmd)
{
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;

    fs::path pth = master_test_suite().argv[1];
    auto env = boost::this_process::environment();

    auto itr = std::find_if(env.begin(), env.end(),
    [](const bp::native_environment::entry_type & e){return boost::to_upper_copy(e.get_name()) == "PATH";});

    BOOST_REQUIRE(itr != env.end());

    (*itr) += fs::canonical(fs::absolute(pth.parent_path())).string();
    BOOST_REQUIRE(itr != env.end());

    bp::system("sparring_partner \"\" ", ec);
}


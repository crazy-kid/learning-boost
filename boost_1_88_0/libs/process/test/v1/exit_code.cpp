// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_TEST_MAIN

#include <boost/test/included/unit_test.hpp>

#include <boost/process/v1/error.hpp>
#include <boost/process/v1/async.hpp>
#include <boost/process/v1/child.hpp>

#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#if defined(BOOST_WINDOWS_API)
#   include <windows.h>
typedef boost::asio::windows::stream_handle pipe_end;
#elif defined(BOOST_POSIX_API)
#   include <sys/wait.h>
#   include <signal.h>
typedef boost::asio::posix::stream_descriptor pipe_end;
#endif

namespace bp = boost::process::v1;

BOOST_AUTO_TEST_CASE(sync_wait, *boost::unit_test::timeout(10))
{
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;
    bp::child c(
        master_test_suite().argv[1],
        "test", "--exit-code", "123",
        ec
    );
    BOOST_REQUIRE(!ec);
    c.wait();
    int exit_code = c.exit_code();


    BOOST_CHECK_EQUAL(123, exit_code);

    c.wait();
}

BOOST_AUTO_TEST_CASE(sync_wait_abort, *boost::unit_test::timeout(10))
{
    using boost::unit_test::framework::master_test_suite;

    std::error_code ec;
    bp::child c(
        master_test_suite().argv[1],
        "test", "--abort",
        ec
    );
    BOOST_REQUIRE(!ec);
    c.wait();
    int exit_code = c.exit_code();


    BOOST_CHECK(exit_code != 0);

    c.wait();
}

#if defined(BOOST_WINDOWS_API)
struct wait_handler
{
    HANDLE handle_;
    bool &called_;
    wait_handler(HANDLE handle, bool &called) : handle_(handle), called_(called) {}

    void operator()(const boost::system::error_code &ec)
    {
        called_ = true;
        BOOST_REQUIRE(!ec);
        DWORD exit_code;
        BOOST_REQUIRE(GetExitCodeProcess(handle_, &exit_code));
        BOOST_CHECK_EQUAL(123, exit_code);
    }
};
#elif defined(BOOST_POSIX_API)
struct wait_handler
{
    bool &called_;

    wait_handler (bool & called) : called_(called) {}

    void operator()(const boost::system::error_code &ec, int signal)
    {
        called_ = true;
        BOOST_REQUIRE(!ec);
        BOOST_REQUIRE_EQUAL(SIGCHLD, signal);
        int status;
        wait(&status);
        BOOST_CHECK_EQUAL(123, WEXITSTATUS(status));
    }
};
#endif

BOOST_AUTO_TEST_CASE(async_wait, *boost::unit_test::timeout(10))
{
    using boost::unit_test::framework::master_test_suite;
    using namespace boost::asio;

    boost::asio::io_context io_context;

    boost::asio::steady_timer timeout{io_context, std::chrono::seconds(3)};
    timeout.async_wait([&](boost::system::error_code ec){if (!ec) io_context.stop();});


    bool wh_called = false;
#if defined(BOOST_POSIX_API)
    signal_set set(io_context, SIGCHLD);
    set.async_wait(wait_handler(wh_called));
#endif

    std::error_code ec;
    bp::child c(
        master_test_suite().argv[1],
        "test", "--exit-code", "123",
        ec
    );
    BOOST_REQUIRE(!ec);

#if defined(BOOST_WINDOWS_API)
    windows::object_handle handle(io_context.get_executor(), c.native_handle());
    handle.async_wait(wait_handler(handle.native_handle(), wh_called));
#endif
    io_context.run();
    BOOST_CHECK_MESSAGE(wh_called, "Wait handler not called");
}



BOOST_AUTO_TEST_CASE(async_nowait, *boost::unit_test::timeout(10))
{
    // No need to call wait when passing an io_context
    using boost::unit_test::framework::master_test_suite;
    using namespace boost::asio;

    std::error_code ec;
    boost::asio::io_context io_context;
    bp::child c(
            master_test_suite().argv[1],
            "test", "--exit-code", "121",
            ec,
            bp::on_exit=[](int exit_code, std::error_code) mutable {},
            io_context
    );
    BOOST_REQUIRE(!ec);
    while (c.running())
    {
      io_context.run_for(std::chrono::milliseconds(10));
      io_context.restart();
    }

    BOOST_CHECK_EQUAL(121, c.exit_code());
}
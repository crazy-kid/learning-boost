//
// overlapped_ptr.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// Disable autolinking for unit tests.
#if !defined(BOOST_ALL_NO_LIB)
#define BOOST_ALL_NO_LIB 1
#endif // !defined(BOOST_ALL_NO_LIB)

// Test that header file is self-contained.
#include <boost/asio/windows/overlapped_ptr.hpp>

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/executor.hpp>
#include <boost/asio/io_context.hpp>
#include "../unit_test.hpp"

//------------------------------------------------------------------------------

// windows_overlapped_ptr_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// windows::overlapped_ptr compile and link correctly. Runtime failures are
// ignored.

namespace windows_overlapped_ptr_compile {

void overlapped_handler_1(const boost::system::error_code&, std::size_t)
{
}

struct overlapped_handler_2
{
  void operator()(const boost::system::error_code&, std::size_t)
  {
  }
};

void test()
{
#if defined(BOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR)
  using namespace boost::asio;
  namespace win = boost::asio::windows;

  try
  {
    io_context ioc;
    boost::asio::any_io_executor ex1(ioc.get_executor());
#if !defined(BOOST_ASIO_NO_TS_EXECUTORS)
    boost::asio::executor ex2(ioc.get_executor());
#endif // !defined(BOOST_ASIO_NO_TS_EXECUTORS)

    // basic_overlapped_ptr constructors.

    win::overlapped_ptr ptr1;

    win::overlapped_ptr ptr2(ioc, &overlapped_handler_1);
    win::overlapped_ptr ptr3(ioc, overlapped_handler_2());

    win::overlapped_ptr ptr4(ioc.get_executor(), &overlapped_handler_1);
    win::overlapped_ptr ptr5(ioc.get_executor(), overlapped_handler_2());
    win::overlapped_ptr ptr6(ex1, &overlapped_handler_1);
    win::overlapped_ptr ptr7(ex1, overlapped_handler_2());
#if !defined(BOOST_ASIO_NO_TS_EXECUTORS)
    win::overlapped_ptr ptr8(ex2, &overlapped_handler_1);
    win::overlapped_ptr ptr9(ex2, overlapped_handler_2());
#endif // !defined(BOOST_ASIO_NO_TS_EXECUTORS)

    // overlapped_ptr functions.

    ptr1.reset();

    ptr2.reset(ioc, &overlapped_handler_1);
    ptr3.reset(ioc, overlapped_handler_2());

    ptr2.reset(ioc.get_executor(), &overlapped_handler_1);
    ptr3.reset(ioc.get_executor(), overlapped_handler_2());
    ptr2.reset(ex1, &overlapped_handler_1);
    ptr3.reset(ex1, overlapped_handler_2());
#if !defined(BOOST_ASIO_NO_TS_EXECUTORS)
    ptr3.reset(ex2, &overlapped_handler_1);
    ptr3.reset(ex2, overlapped_handler_2());
#endif // !defined(BOOST_ASIO_NO_TS_EXECUTORS)

    OVERLAPPED* ov1 = ptr1.get();
    (void)ov1;

    const win::overlapped_ptr& ptr10(ptr1);
    const OVERLAPPED* ov2 = ptr10.get();
    (void)ov2;

    OVERLAPPED* ov3 = ptr1.release();
    (void)ov3;

    boost::system::error_code ec;
    std::size_t bytes_transferred = 0;
    ptr1.complete(ec, bytes_transferred);
  }
  catch (std::exception&)
  {
  }
#endif // defined(BOOST_ASIO_HAS_WINDOWS_OVERLAPPED_PTR)
}

} // namespace windows_overlapped_ptr_compile

//------------------------------------------------------------------------------

BOOST_ASIO_TEST_SUITE
(
  "windows/overlapped_ptr",
  BOOST_ASIO_COMPILE_TEST_CASE(windows_overlapped_ptr_compile::test)
)

//
// tcp.cpp
// ~~~~~~~
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

// Enable cancel() support on Windows.
#define BOOST_ASIO_ENABLE_CANCELIO 1

// Test that header file is self-contained.
#include <boost/asio/ip/tcp.hpp>

#include <cstring>
#include <functional>
#include <boost/asio/io_context.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include "../unit_test.hpp"
#include "../archetypes/async_result.hpp"
#include "../archetypes/gettable_socket_option.hpp"
#include "../archetypes/io_control_command.hpp"
#include "../archetypes/settable_socket_option.hpp"

#if defined(BOOST_ASIO_HAS_BOOST_ARRAY)
# include <boost/array.hpp>
#else // defined(BOOST_ASIO_HAS_BOOST_ARRAY)
# include <array>
#endif // defined(BOOST_ASIO_HAS_BOOST_ARRAY)

//------------------------------------------------------------------------------

// ip_tcp_compile test
// ~~~~~~~~~~~~~~~~~~~
// The following test checks that all nested classes, enums and constants in
// ip::tcp compile and link correctly. Runtime failures are ignored.

namespace ip_tcp_compile {

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    ip::tcp::socket sock(ioc);

    // no_delay class.

    ip::tcp::no_delay no_delay1(true);
    sock.set_option(no_delay1);
    ip::tcp::no_delay no_delay2;
    sock.get_option(no_delay2);
    no_delay1 = true;
    (void)static_cast<bool>(no_delay1);
    (void)static_cast<bool>(!no_delay1);
    (void)static_cast<bool>(no_delay1.value());

    ip::tcp::endpoint ep;
    (void)static_cast<std::size_t>(std::hash<ip::tcp::endpoint>()(ep));
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_tcp_compile

//------------------------------------------------------------------------------

// ip_tcp_runtime test
// ~~~~~~~~~~~~~~~~~~~
// The following test checks the runtime operation of the ip::tcp class.

namespace ip_tcp_runtime {

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  io_context ioc;
  ip::tcp::socket sock(ioc, ip::tcp::v4());
  boost::system::error_code ec;

  // no_delay class.

  ip::tcp::no_delay no_delay1(true);
  BOOST_ASIO_CHECK(no_delay1.value());
  BOOST_ASIO_CHECK(static_cast<bool>(no_delay1));
  BOOST_ASIO_CHECK(!!no_delay1);
  sock.set_option(no_delay1, ec);
  BOOST_ASIO_CHECK(!ec);

  ip::tcp::no_delay no_delay2;
  sock.get_option(no_delay2, ec);
  BOOST_ASIO_CHECK(!ec);
  BOOST_ASIO_CHECK(no_delay2.value());
  BOOST_ASIO_CHECK(static_cast<bool>(no_delay2));
  BOOST_ASIO_CHECK(!!no_delay2);

  ip::tcp::no_delay no_delay3(false);
  BOOST_ASIO_CHECK(!no_delay3.value());
  BOOST_ASIO_CHECK(!static_cast<bool>(no_delay3));
  BOOST_ASIO_CHECK(!no_delay3);
  sock.set_option(no_delay3, ec);
  BOOST_ASIO_CHECK(!ec);

  ip::tcp::no_delay no_delay4;
  sock.get_option(no_delay4, ec);
  BOOST_ASIO_CHECK(!ec);
  BOOST_ASIO_CHECK(!no_delay4.value());
  BOOST_ASIO_CHECK(!static_cast<bool>(no_delay4));
  BOOST_ASIO_CHECK(!no_delay4);
}

} // namespace ip_tcp_runtime

//------------------------------------------------------------------------------

// ip_tcp_socket_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::tcp::socket compile and link correctly. Runtime failures are ignored.

namespace ip_tcp_socket_compile {

struct connect_handler
{
  connect_handler() {}
  void operator()(const boost::system::error_code&) {}
  connect_handler(connect_handler&&) {}
private:
  connect_handler(const connect_handler&);
};

struct wait_handler
{
  wait_handler() {}
  void operator()(const boost::system::error_code&) {}
  wait_handler(wait_handler&&) {}
private:
  wait_handler(const wait_handler&);
};

struct send_handler
{
  send_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  send_handler(send_handler&&) {}
private:
  send_handler(const send_handler&);
};

struct receive_handler
{
  receive_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  receive_handler(receive_handler&&) {}
private:
  receive_handler(const receive_handler&);
};

struct write_some_handler
{
  write_some_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  write_some_handler(write_some_handler&&) {}
private:
  write_some_handler(const write_some_handler&);
};

struct read_some_handler
{
  read_some_handler() {}
  void operator()(const boost::system::error_code&, std::size_t) {}
  read_some_handler(read_some_handler&&) {}
private:
  read_some_handler(const read_some_handler&);
};

void test()
{
#if defined(BOOST_ASIO_HAS_BOOST_ARRAY)
  using boost::array;
#else // defined(BOOST_ASIO_HAS_BOOST_ARRAY)
  using std::array;
#endif // defined(BOOST_ASIO_HAS_BOOST_ARRAY)

  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    const io_context::executor_type ioc_ex = ioc.get_executor();
    char mutable_char_buffer[128] = "";
    const char const_char_buffer[128] = "";
    array<boost::asio::mutable_buffer, 2> mutable_buffers = {{
        boost::asio::buffer(mutable_char_buffer, 10),
        boost::asio::buffer(mutable_char_buffer + 10, 10) }};
    array<boost::asio::const_buffer, 2> const_buffers = {{
        boost::asio::buffer(const_char_buffer, 10),
        boost::asio::buffer(const_char_buffer + 10, 10) }};
    socket_base::message_flags in_flags = 0;
    archetypes::settable_socket_option<void> settable_socket_option1;
    archetypes::settable_socket_option<int> settable_socket_option2;
    archetypes::settable_socket_option<double> settable_socket_option3;
    archetypes::gettable_socket_option<void> gettable_socket_option1;
    archetypes::gettable_socket_option<int> gettable_socket_option2;
    archetypes::gettable_socket_option<double> gettable_socket_option3;
    archetypes::io_control_command io_control_command;
    archetypes::immediate_handler immediate;
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;

    // basic_stream_socket constructors.

    ip::tcp::socket socket1(ioc);
    ip::tcp::socket socket2(ioc, ip::tcp::v4());
    ip::tcp::socket socket3(ioc, ip::tcp::v6());
    ip::tcp::socket socket4(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
    ip::tcp::socket socket5(ioc, ip::tcp::endpoint(ip::tcp::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::socket::native_handle_type native_socket1
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ip::tcp::socket socket6(ioc, ip::tcp::v4(), native_socket1);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::tcp::socket socket7(ioc_ex);
    ip::tcp::socket socket8(ioc_ex, ip::tcp::v4());
    ip::tcp::socket socket9(ioc_ex, ip::tcp::v6());
    ip::tcp::socket socket10(ioc_ex, ip::tcp::endpoint(ip::tcp::v4(), 0));
    ip::tcp::socket socket11(ioc_ex, ip::tcp::endpoint(ip::tcp::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::socket::native_handle_type native_socket2
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ip::tcp::socket socket12(ioc_ex, ip::tcp::v4(), native_socket2);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::tcp::socket socket13(std::move(socket5));

    // basic_stream_socket operators.

    socket1 = ip::tcp::socket(ioc);
    socket1 = std::move(socket2);

    // I/O object functions.

    ip::tcp::socket::executor_type ex = socket1.get_executor();
    (void)ex;

    // basic_socket functions.

    ip::tcp::socket::lowest_layer_type& lowest_layer = socket1.lowest_layer();
    (void)lowest_layer;

    const ip::tcp::socket& socket14 = socket1;
    const ip::tcp::socket::lowest_layer_type& lowest_layer2
      = socket14.lowest_layer();
    (void)lowest_layer2;

    socket1.open(ip::tcp::v4());
    socket1.open(ip::tcp::v6());
    socket1.open(ip::tcp::v4(), ec);
    socket1.open(ip::tcp::v6(), ec);

#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::socket::native_handle_type native_socket3
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    socket1.assign(ip::tcp::v4(), native_socket3);
    ip::tcp::socket::native_handle_type native_socket4
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    socket1.assign(ip::tcp::v4(), native_socket4, ec);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    bool is_open = socket1.is_open();
    (void)is_open;

    socket1.close();
    socket1.close(ec);

    socket1.release();
    socket1.release(ec);

    ip::tcp::socket::native_handle_type native_socket5
      = socket1.native_handle();
    (void)native_socket5;

    socket1.cancel();
    socket1.cancel(ec);

    bool at_mark1 = socket1.at_mark();
    (void)at_mark1;
    bool at_mark2 = socket1.at_mark(ec);
    (void)at_mark2;

    std::size_t available1 = socket1.available();
    (void)available1;
    std::size_t available2 = socket1.available(ec);
    (void)available2;

    socket1.bind(ip::tcp::endpoint(ip::tcp::v4(), 0));
    socket1.bind(ip::tcp::endpoint(ip::tcp::v6(), 0));
    socket1.bind(ip::tcp::endpoint(ip::tcp::v4(), 0), ec);
    socket1.bind(ip::tcp::endpoint(ip::tcp::v6(), 0), ec);

    socket1.connect(ip::tcp::endpoint(ip::tcp::v4(), 0));
    socket1.connect(ip::tcp::endpoint(ip::tcp::v6(), 0));
    socket1.connect(ip::tcp::endpoint(ip::tcp::v4(), 0), ec);
    socket1.connect(ip::tcp::endpoint(ip::tcp::v6(), 0), ec);

    socket1.async_connect(ip::tcp::endpoint(ip::tcp::v4(), 0),
        connect_handler());
    socket1.async_connect(ip::tcp::endpoint(ip::tcp::v6(), 0),
        connect_handler());
    socket1.async_connect(ip::tcp::endpoint(ip::tcp::v4(), 0), immediate);
    socket1.async_connect(ip::tcp::endpoint(ip::tcp::v6(), 0), immediate);
    int i1 = socket1.async_connect(ip::tcp::endpoint(ip::tcp::v4(), 0), lazy);
    (void)i1;
    int i2 = socket1.async_connect(ip::tcp::endpoint(ip::tcp::v6(), 0), lazy);
    (void)i2;

    socket1.set_option(settable_socket_option1);
    socket1.set_option(settable_socket_option1, ec);
    socket1.set_option(settable_socket_option2);
    socket1.set_option(settable_socket_option2, ec);
    socket1.set_option(settable_socket_option3);
    socket1.set_option(settable_socket_option3, ec);

    socket1.get_option(gettable_socket_option1);
    socket1.get_option(gettable_socket_option1, ec);
    socket1.get_option(gettable_socket_option2);
    socket1.get_option(gettable_socket_option2, ec);
    socket1.get_option(gettable_socket_option3);
    socket1.get_option(gettable_socket_option3, ec);

    socket1.io_control(io_control_command);
    socket1.io_control(io_control_command, ec);

    bool non_blocking1 = socket1.non_blocking();
    (void)non_blocking1;
    socket1.non_blocking(true);
    socket1.non_blocking(false, ec);

    bool non_blocking2 = socket1.native_non_blocking();
    (void)non_blocking2;
    socket1.native_non_blocking(true);
    socket1.native_non_blocking(false, ec);

    ip::tcp::endpoint endpoint1 = socket1.local_endpoint();
    (void)endpoint1;
    ip::tcp::endpoint endpoint2 = socket1.local_endpoint(ec);
    (void)endpoint2;

    ip::tcp::endpoint endpoint3 = socket1.remote_endpoint();
    (void)endpoint3;
    ip::tcp::endpoint endpoint4 = socket1.remote_endpoint(ec);
    (void)endpoint4;

    socket1.shutdown(socket_base::shutdown_both);
    socket1.shutdown(socket_base::shutdown_both, ec);

    socket1.wait(socket_base::wait_read);
    socket1.wait(socket_base::wait_write, ec);

    socket1.async_wait(socket_base::wait_read, wait_handler());
    socket1.async_wait(socket_base::wait_read, immediate);
    int i3 = socket1.async_wait(socket_base::wait_write, lazy);
    (void)i3;

    // basic_stream_socket functions.

    socket1.send(buffer(mutable_char_buffer));
    socket1.send(buffer(const_char_buffer));
    socket1.send(mutable_buffers);
    socket1.send(const_buffers);
    socket1.send(null_buffers());
    socket1.send(buffer(mutable_char_buffer), in_flags);
    socket1.send(buffer(const_char_buffer), in_flags);
    socket1.send(mutable_buffers, in_flags);
    socket1.send(const_buffers, in_flags);
    socket1.send(null_buffers(), in_flags);
    socket1.send(buffer(mutable_char_buffer), in_flags, ec);
    socket1.send(buffer(const_char_buffer), in_flags, ec);
    socket1.send(mutable_buffers, in_flags, ec);
    socket1.send(const_buffers, in_flags, ec);
    socket1.send(null_buffers(), in_flags, ec);

    socket1.async_send(buffer(mutable_char_buffer), send_handler());
    socket1.async_send(buffer(const_char_buffer), send_handler());
    socket1.async_send(mutable_buffers, send_handler());
    socket1.async_send(const_buffers, send_handler());
    socket1.async_send(null_buffers(), send_handler());
    socket1.async_send(buffer(mutable_char_buffer), in_flags, send_handler());
    socket1.async_send(buffer(const_char_buffer), in_flags, send_handler());
    socket1.async_send(mutable_buffers, in_flags, send_handler());
    socket1.async_send(const_buffers, in_flags, send_handler());
    socket1.async_send(null_buffers(), in_flags, send_handler());
    socket1.async_send(buffer(mutable_char_buffer), immediate);
    socket1.async_send(buffer(const_char_buffer), immediate);
    socket1.async_send(mutable_buffers, immediate);
    socket1.async_send(const_buffers, immediate);
    socket1.async_send(null_buffers(), immediate);
    socket1.async_send(buffer(mutable_char_buffer), in_flags, immediate);
    socket1.async_send(buffer(const_char_buffer), in_flags, immediate);
    socket1.async_send(mutable_buffers, in_flags, immediate);
    socket1.async_send(const_buffers, in_flags, immediate);
    socket1.async_send(null_buffers(), in_flags, immediate);
    int i4 = socket1.async_send(buffer(mutable_char_buffer), lazy);
    (void)i4;
    int i5 = socket1.async_send(buffer(const_char_buffer), lazy);
    (void)i5;
    int i6 = socket1.async_send(mutable_buffers, lazy);
    (void)i6;
    int i7 = socket1.async_send(const_buffers, lazy);
    (void)i7;
    int i8 = socket1.async_send(null_buffers(), lazy);
    (void)i8;
    int i9 = socket1.async_send(buffer(mutable_char_buffer), in_flags, lazy);
    (void)i9;
    int i10 = socket1.async_send(buffer(const_char_buffer), in_flags, lazy);
    (void)i10;
    int i11 = socket1.async_send(mutable_buffers, in_flags, lazy);
    (void)i11;
    int i12 = socket1.async_send(const_buffers, in_flags, lazy);
    (void)i12;
    int i13 = socket1.async_send(null_buffers(), in_flags, lazy);
    (void)i13;

    socket1.receive(buffer(mutable_char_buffer));
    socket1.receive(mutable_buffers);
    socket1.receive(null_buffers());
    socket1.receive(buffer(mutable_char_buffer), in_flags);
    socket1.receive(mutable_buffers, in_flags);
    socket1.receive(null_buffers(), in_flags);
    socket1.receive(buffer(mutable_char_buffer), in_flags, ec);
    socket1.receive(mutable_buffers, in_flags, ec);
    socket1.receive(null_buffers(), in_flags, ec);

    socket1.async_receive(buffer(mutable_char_buffer), receive_handler());
    socket1.async_receive(mutable_buffers, receive_handler());
    socket1.async_receive(null_buffers(), receive_handler());
    socket1.async_receive(buffer(mutable_char_buffer), in_flags,
        receive_handler());
    socket1.async_receive(mutable_buffers, in_flags, receive_handler());
    socket1.async_receive(null_buffers(), in_flags, receive_handler());
    socket1.async_receive(buffer(mutable_char_buffer), immediate);
    socket1.async_receive(mutable_buffers, immediate);
    socket1.async_receive(null_buffers(), immediate);
    socket1.async_receive(buffer(mutable_char_buffer), in_flags, immediate);
    socket1.async_receive(mutable_buffers, in_flags, immediate);
    socket1.async_receive(null_buffers(), in_flags, immediate);
    int i14 = socket1.async_receive(buffer(mutable_char_buffer), lazy);
    (void)i14;
    int i15 = socket1.async_receive(mutable_buffers, lazy);
    (void)i15;
    int i16 = socket1.async_receive(null_buffers(), lazy);
    (void)i16;
    int i17 = socket1.async_receive(buffer(mutable_char_buffer), in_flags,
        lazy);
    (void)i17;
    int i18 = socket1.async_receive(mutable_buffers, in_flags, lazy);
    (void)i18;
    int i19 = socket1.async_receive(null_buffers(), in_flags, lazy);
    (void)i19;

    socket1.write_some(buffer(mutable_char_buffer));
    socket1.write_some(buffer(const_char_buffer));
    socket1.write_some(mutable_buffers);
    socket1.write_some(const_buffers);
    socket1.write_some(null_buffers());
    socket1.write_some(buffer(mutable_char_buffer), ec);
    socket1.write_some(buffer(const_char_buffer), ec);
    socket1.write_some(mutable_buffers, ec);
    socket1.write_some(const_buffers, ec);
    socket1.write_some(null_buffers(), ec);

    socket1.async_write_some(buffer(mutable_char_buffer), write_some_handler());
    socket1.async_write_some(buffer(const_char_buffer), write_some_handler());
    socket1.async_write_some(mutable_buffers, write_some_handler());
    socket1.async_write_some(const_buffers, write_some_handler());
    socket1.async_write_some(null_buffers(), write_some_handler());
    socket1.async_write_some(buffer(mutable_char_buffer), immediate);
    socket1.async_write_some(buffer(const_char_buffer), immediate);
    socket1.async_write_some(mutable_buffers, immediate);
    socket1.async_write_some(const_buffers, immediate);
    socket1.async_write_some(null_buffers(), immediate);
    int i20 = socket1.async_write_some(buffer(mutable_char_buffer), lazy);
    (void)i20;
    int i21 = socket1.async_write_some(buffer(const_char_buffer), lazy);
    (void)i21;
    int i22 = socket1.async_write_some(mutable_buffers, lazy);
    (void)i22;
    int i23 = socket1.async_write_some(const_buffers, lazy);
    (void)i23;
    int i24 = socket1.async_write_some(null_buffers(), lazy);
    (void)i24;

    socket1.read_some(buffer(mutable_char_buffer));
    socket1.read_some(mutable_buffers);
    socket1.read_some(null_buffers());
    socket1.read_some(buffer(mutable_char_buffer), ec);
    socket1.read_some(mutable_buffers, ec);
    socket1.read_some(null_buffers(), ec);

    socket1.async_read_some(buffer(mutable_char_buffer), read_some_handler());
    socket1.async_read_some(mutable_buffers, read_some_handler());
    socket1.async_read_some(null_buffers(), read_some_handler());
    socket1.async_read_some(buffer(mutable_char_buffer), immediate);
    socket1.async_read_some(mutable_buffers, immediate);
    socket1.async_read_some(null_buffers(), immediate);
    int i25 = socket1.async_read_some(buffer(mutable_char_buffer), lazy);
    (void)i25;
    int i26 = socket1.async_read_some(mutable_buffers, lazy);
    (void)i26;
    int i27 = socket1.async_read_some(null_buffers(), lazy);
    (void)i27;
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_tcp_socket_compile

//------------------------------------------------------------------------------

// ip_tcp_socket_runtime test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks the runtime operation of the ip::tcp::socket class.

namespace ip_tcp_socket_runtime {

static const char write_data[]
  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void handle_read_noop(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(!err);
  BOOST_ASIO_CHECK(bytes_transferred == 0);
}

void handle_write_noop(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(!err);
  BOOST_ASIO_CHECK(bytes_transferred == 0);
}

void handle_read(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(!err);
  BOOST_ASIO_CHECK(bytes_transferred == sizeof(write_data));
}

void handle_write(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(!err);
  BOOST_ASIO_CHECK(bytes_transferred == sizeof(write_data));
}

void handle_read_cancel(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(err == boost::asio::error::operation_aborted);
  BOOST_ASIO_CHECK(bytes_transferred == 0);
}

void handle_read_eof(const boost::system::error_code& err,
    size_t bytes_transferred, bool* called)
{
  *called = true;
  BOOST_ASIO_CHECK(err == boost::asio::error::eof);
  BOOST_ASIO_CHECK(bytes_transferred == 0);
}

void test()
{
  using namespace std; // For memcmp.
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  namespace bindns = std;
  using bindns::placeholders::_1;
  using bindns::placeholders::_2;

  io_context ioc;

  ip::tcp::acceptor acceptor(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
  ip::tcp::endpoint server_endpoint = acceptor.local_endpoint();
  server_endpoint.address(ip::address_v4::loopback());

  ip::tcp::socket client_side_socket(ioc);
  ip::tcp::socket server_side_socket(ioc);

  client_side_socket.connect(server_endpoint);
  acceptor.accept(server_side_socket);

  // No-op read.

  bool read_noop_completed = false;
  client_side_socket.async_read_some(
      boost::asio::mutable_buffer(0, 0),
      bindns::bind(handle_read_noop,
        _1, _2, &read_noop_completed));

  ioc.run();
  BOOST_ASIO_CHECK(read_noop_completed);

  // No-op write.

  bool write_noop_completed = false;
  client_side_socket.async_write_some(
      boost::asio::const_buffer(0, 0),
      bindns::bind(handle_write_noop,
        _1, _2, &write_noop_completed));

  ioc.restart();
  ioc.run();
  BOOST_ASIO_CHECK(write_noop_completed);

  // Read and write to transfer data.

  char read_buffer[sizeof(write_data)];
  bool read_completed = false;
  boost::asio::async_read(client_side_socket,
      boost::asio::buffer(read_buffer),
      bindns::bind(handle_read,
        _1, _2, &read_completed));

  bool write_completed = false;
  boost::asio::async_write(server_side_socket,
      boost::asio::buffer(write_data),
      bindns::bind(handle_write,
        _1, _2, &write_completed));

  ioc.restart();
  ioc.run();
  BOOST_ASIO_CHECK(read_completed);
  BOOST_ASIO_CHECK(write_completed);
  BOOST_ASIO_CHECK(memcmp(read_buffer, write_data, sizeof(write_data)) == 0);

  // Cancelled read.

  bool read_cancel_completed = false;
  boost::asio::async_read(server_side_socket,
      boost::asio::buffer(read_buffer),
      bindns::bind(handle_read_cancel,
        _1, _2, &read_cancel_completed));

  ioc.restart();
  ioc.poll();
  BOOST_ASIO_CHECK(!read_cancel_completed);

  server_side_socket.cancel();

  ioc.restart();
  ioc.run();
  BOOST_ASIO_CHECK(read_cancel_completed);

  // A read when the peer closes socket should fail with eof.

  bool read_eof_completed = false;
  boost::asio::async_read(client_side_socket,
      boost::asio::buffer(read_buffer),
      bindns::bind(handle_read_eof,
        _1, _2, &read_eof_completed));

  server_side_socket.close();

  ioc.restart();
  ioc.run();
  BOOST_ASIO_CHECK(read_eof_completed);
}

} // namespace ip_tcp_socket_runtime

//------------------------------------------------------------------------------

// ip_tcp_acceptor_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::tcp::acceptor compile and link correctly. Runtime failures are ignored.

namespace ip_tcp_acceptor_compile {

struct wait_handler
{
  wait_handler() {}
  void operator()(const boost::system::error_code&) {}
  wait_handler(wait_handler&&) {}
private:
  wait_handler(const wait_handler&);
};

struct accept_handler
{
  accept_handler() {}
  void operator()(const boost::system::error_code&) {}
  accept_handler(accept_handler&&) {}
private:
  accept_handler(const accept_handler&);
};

struct move_accept_handler
{
  move_accept_handler() {}
  void operator()(
      const boost::system::error_code&, boost::asio::ip::tcp::socket) {}
  move_accept_handler(move_accept_handler&&) {}
private:
  move_accept_handler(const move_accept_handler&) {}
};

struct move_accept_ioc_handler
{
  move_accept_ioc_handler() {}
  void operator()(const boost::system::error_code&,
      boost::asio::basic_stream_socket<boost::asio::ip::tcp,
        boost::asio::io_context::executor_type>) {}
  move_accept_ioc_handler(move_accept_handler&&) {}
private:
  move_accept_ioc_handler(const move_accept_handler&) {}
};

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    const io_context::executor_type ioc_ex = ioc.get_executor();
    ip::tcp::socket peer_socket1(ioc);
    boost::asio::basic_stream_socket<ip::tcp,
        io_context::executor_type> peer_socket2(ioc);
    ip::tcp::endpoint peer_endpoint;
    archetypes::settable_socket_option<void> settable_socket_option1;
    archetypes::settable_socket_option<int> settable_socket_option2;
    archetypes::settable_socket_option<double> settable_socket_option3;
    archetypes::gettable_socket_option<void> gettable_socket_option1;
    archetypes::gettable_socket_option<int> gettable_socket_option2;
    archetypes::gettable_socket_option<double> gettable_socket_option3;
    archetypes::io_control_command io_control_command;
    archetypes::immediate_handler immediate;
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;

    // basic_socket_acceptor constructors.

    ip::tcp::acceptor acceptor1(ioc);
    ip::tcp::acceptor acceptor2(ioc, ip::tcp::v4());
    ip::tcp::acceptor acceptor3(ioc, ip::tcp::v6());
    ip::tcp::acceptor acceptor4(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
    ip::tcp::acceptor acceptor5(ioc, ip::tcp::endpoint(ip::tcp::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::acceptor::native_handle_type native_acceptor1
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ip::tcp::acceptor acceptor6(ioc, ip::tcp::v4(), native_acceptor1);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::tcp::acceptor acceptor7(ioc_ex);
    ip::tcp::acceptor acceptor8(ioc_ex, ip::tcp::v4());
    ip::tcp::acceptor acceptor9(ioc_ex, ip::tcp::v6());
    ip::tcp::acceptor acceptor10(ioc_ex, ip::tcp::endpoint(ip::tcp::v4(), 0));
    ip::tcp::acceptor acceptor11(ioc_ex, ip::tcp::endpoint(ip::tcp::v6(), 0));
#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::acceptor::native_handle_type native_acceptor2
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ip::tcp::acceptor acceptor12(ioc_ex, ip::tcp::v4(), native_acceptor2);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    ip::tcp::acceptor acceptor13(std::move(acceptor5));

    // basic_socket_acceptor operators.

    acceptor1 = ip::tcp::acceptor(ioc);
    acceptor1 = std::move(acceptor2);

    // I/O object functions.

    ip::tcp::acceptor::executor_type ex = acceptor1.get_executor();
    (void)ex;

    // basic_socket_acceptor functions.

    acceptor1.open(ip::tcp::v4());
    acceptor1.open(ip::tcp::v6());
    acceptor1.open(ip::tcp::v4(), ec);
    acceptor1.open(ip::tcp::v6(), ec);

#if !defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ip::tcp::acceptor::native_handle_type native_acceptor3
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    acceptor1.assign(ip::tcp::v4(), native_acceptor3);
    ip::tcp::acceptor::native_handle_type native_acceptor4
      = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    acceptor1.assign(ip::tcp::v4(), native_acceptor4, ec);
#endif // !defined(BOOST_ASIO_WINDOWS_RUNTIME)

    bool is_open = acceptor1.is_open();
    (void)is_open;

    acceptor1.close();
    acceptor1.close(ec);

    acceptor1.release();
    acceptor1.release(ec);

    ip::tcp::acceptor::native_handle_type native_acceptor5
      = acceptor1.native_handle();
    (void)native_acceptor5;

    acceptor1.cancel();
    acceptor1.cancel(ec);

    acceptor1.bind(ip::tcp::endpoint(ip::tcp::v4(), 0));
    acceptor1.bind(ip::tcp::endpoint(ip::tcp::v6(), 0));
    acceptor1.bind(ip::tcp::endpoint(ip::tcp::v4(), 0), ec);
    acceptor1.bind(ip::tcp::endpoint(ip::tcp::v6(), 0), ec);

    acceptor1.set_option(settable_socket_option1);
    acceptor1.set_option(settable_socket_option1, ec);
    acceptor1.set_option(settable_socket_option2);
    acceptor1.set_option(settable_socket_option2, ec);
    acceptor1.set_option(settable_socket_option3);
    acceptor1.set_option(settable_socket_option3, ec);

    acceptor1.get_option(gettable_socket_option1);
    acceptor1.get_option(gettable_socket_option1, ec);
    acceptor1.get_option(gettable_socket_option2);
    acceptor1.get_option(gettable_socket_option2, ec);
    acceptor1.get_option(gettable_socket_option3);
    acceptor1.get_option(gettable_socket_option3, ec);

    acceptor1.io_control(io_control_command);
    acceptor1.io_control(io_control_command, ec);

    bool non_blocking1 = acceptor1.non_blocking();
    (void)non_blocking1;
    acceptor1.non_blocking(true);
    acceptor1.non_blocking(false, ec);

    bool non_blocking2 = acceptor1.native_non_blocking();
    (void)non_blocking2;
    acceptor1.native_non_blocking(true);
    acceptor1.native_non_blocking(false, ec);

    ip::tcp::endpoint endpoint1 = acceptor1.local_endpoint();
    (void)endpoint1;
    ip::tcp::endpoint endpoint2 = acceptor1.local_endpoint(ec);
    (void)endpoint2;

    acceptor1.wait(socket_base::wait_read);
    acceptor1.wait(socket_base::wait_write, ec);

    acceptor1.async_wait(socket_base::wait_read, wait_handler());
    acceptor1.async_wait(socket_base::wait_read, immediate);
    int i1 = acceptor1.async_wait(socket_base::wait_write, lazy);
    (void)i1;

    acceptor1.accept(peer_socket1);
    acceptor1.accept(peer_socket1, ec);
    acceptor1.accept(peer_socket1, peer_endpoint);
    acceptor1.accept(peer_socket1, peer_endpoint, ec);

    acceptor1.accept(peer_socket2);
    acceptor1.accept(peer_socket2, ec);
    acceptor1.accept(peer_socket2, peer_endpoint);
    acceptor1.accept(peer_socket2, peer_endpoint, ec);

    peer_socket1 = acceptor1.accept();
    peer_socket1 = acceptor1.accept(ioc);
    peer_socket1 = acceptor1.accept(ioc_ex);
    peer_socket1 = acceptor1.accept(peer_endpoint);
    peer_socket1 = acceptor1.accept(ioc, peer_endpoint);
    peer_socket1 = acceptor1.accept(ioc_ex, peer_endpoint);
    (void)peer_socket1;

    peer_socket2 = acceptor1.accept(ioc);
    peer_socket2 = acceptor1.accept(ioc_ex);
    peer_socket2 = acceptor1.accept(ioc, peer_endpoint);
    peer_socket2 = acceptor1.accept(ioc_ex, peer_endpoint);
    (void)peer_socket2;

    acceptor1.async_accept(peer_socket1, accept_handler());
    acceptor1.async_accept(peer_socket1, peer_endpoint, accept_handler());
    acceptor1.async_accept(peer_socket1, immediate);
    acceptor1.async_accept(peer_socket1, peer_endpoint, immediate);
    int i2 = acceptor1.async_accept(peer_socket1, lazy);
    (void)i2;
    int i3 = acceptor1.async_accept(peer_socket1, peer_endpoint, lazy);
    (void)i3;

    acceptor1.async_accept(peer_socket2, accept_handler());
    acceptor1.async_accept(peer_socket2, peer_endpoint, accept_handler());
    acceptor1.async_accept(peer_socket2, immediate);
    acceptor1.async_accept(peer_socket2, peer_endpoint, immediate);
    int i4 = acceptor1.async_accept(peer_socket2, lazy);
    (void)i4;
    int i5 = acceptor1.async_accept(peer_socket2, peer_endpoint, lazy);
    (void)i5;

    acceptor1.async_accept(move_accept_handler());
    acceptor1.async_accept(ioc, move_accept_handler());
    acceptor1.async_accept(ioc_ex, move_accept_handler());
    acceptor1.async_accept(ioc_ex, move_accept_ioc_handler());
    acceptor1.async_accept(peer_endpoint, move_accept_handler());
    acceptor1.async_accept(ioc, peer_endpoint, move_accept_handler());
    acceptor1.async_accept(ioc_ex, peer_endpoint, move_accept_handler());
    acceptor1.async_accept(ioc_ex, peer_endpoint, move_accept_ioc_handler());
    acceptor1.async_accept(immediate);
    acceptor1.async_accept(ioc, immediate);
    acceptor1.async_accept(ioc_ex, immediate);
    acceptor1.async_accept(peer_endpoint, immediate);
    acceptor1.async_accept(ioc, peer_endpoint, immediate);
    acceptor1.async_accept(ioc_ex, peer_endpoint, immediate);
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_tcp_acceptor_compile

//------------------------------------------------------------------------------

// ip_tcp_acceptor_runtime test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks the runtime operation of the ip::tcp::acceptor
// class.

namespace ip_tcp_acceptor_runtime {

void handle_accept(const boost::system::error_code& err)
{
  BOOST_ASIO_CHECK(!err);
}

void handle_connect(const boost::system::error_code& err)
{
  BOOST_ASIO_CHECK(!err);
}

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  io_context ioc;

  ip::tcp::acceptor acceptor(ioc, ip::tcp::endpoint(ip::tcp::v4(), 0));
  ip::tcp::endpoint server_endpoint = acceptor.local_endpoint();
  server_endpoint.address(ip::address_v4::loopback());

  ip::tcp::socket client_side_socket(ioc);
  ip::tcp::socket server_side_socket(ioc);

  client_side_socket.connect(server_endpoint);
  acceptor.accept(server_side_socket);

  client_side_socket.close();
  server_side_socket.close();

  client_side_socket.connect(server_endpoint);
  ip::tcp::endpoint client_endpoint;
  acceptor.accept(server_side_socket, client_endpoint);

  ip::tcp::endpoint client_side_local_endpoint
    = client_side_socket.local_endpoint();
  BOOST_ASIO_CHECK(client_side_local_endpoint.port() == client_endpoint.port());

  ip::tcp::endpoint server_side_remote_endpoint
    = server_side_socket.remote_endpoint();
  BOOST_ASIO_CHECK(server_side_remote_endpoint.port()
      == client_endpoint.port());

  client_side_socket.close();
  server_side_socket.close();

  acceptor.async_accept(server_side_socket, &handle_accept);
  client_side_socket.async_connect(server_endpoint, &handle_connect);

  ioc.run();

  client_side_socket.close();
  server_side_socket.close();

  acceptor.async_accept(server_side_socket, client_endpoint, &handle_accept);
  client_side_socket.async_connect(server_endpoint, &handle_connect);

  ioc.restart();
  ioc.run();

  client_side_local_endpoint = client_side_socket.local_endpoint();
  BOOST_ASIO_CHECK(client_side_local_endpoint.port() == client_endpoint.port());

  server_side_remote_endpoint = server_side_socket.remote_endpoint();
  BOOST_ASIO_CHECK(server_side_remote_endpoint.port()
      == client_endpoint.port());
}

} // namespace ip_tcp_acceptor_runtime

//------------------------------------------------------------------------------

// ip_tcp_resolver_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::tcp::resolver compile and link correctly. Runtime failures are ignored.

namespace ip_tcp_resolver_compile {

struct resolve_handler
{
  resolve_handler() {}
  void operator()(const boost::system::error_code&,
      boost::asio::ip::tcp::resolver::results_type) {}
  resolve_handler(resolve_handler&&) {}
private:
  resolve_handler(const resolve_handler&);
};

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  try
  {
    io_context ioc;
    const io_context::executor_type ioc_ex = ioc.get_executor();
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;
    ip::tcp::endpoint e(ip::address_v4::loopback(), 0);

    // basic_resolver constructors.

    ip::tcp::resolver resolver(ioc);
    ip::tcp::resolver resolver2(ioc_ex);

    ip::tcp::resolver resolver3(std::move(resolver));

    // basic_resolver operators.

    resolver = ip::tcp::resolver(ioc);
    resolver = std::move(resolver3);

    // I/O object functions.

    ip::tcp::resolver::executor_type ex = resolver.get_executor();
    (void)ex;

    // basic_resolver functions.

    resolver.cancel();

    ip::tcp::resolver::results_type results1 = resolver.resolve("", "");
    (void)results1;

    ip::tcp::resolver::results_type results2 = resolver.resolve("", "", ec);
    (void)results2;

    ip::tcp::resolver::results_type results3 =
      resolver.resolve("", "", ip::tcp::resolver::flags());
    (void)results3;

    ip::tcp::resolver::results_type results4 =
      resolver.resolve("", "", ip::tcp::resolver::flags(), ec);
    (void)results4;

    ip::tcp::resolver::results_type results5 =
      resolver.resolve(ip::tcp::v4(), "", "");
    (void)results5;

    ip::tcp::resolver::results_type results6 =
      resolver.resolve(ip::tcp::v4(), "", "", ec);
    (void)results6;

    ip::tcp::resolver::results_type results7 =
      resolver.resolve(ip::tcp::v4(), "", "", ip::tcp::resolver::flags());
    (void)results7;

    ip::tcp::resolver::results_type results8 =
      resolver.resolve(ip::tcp::v4(), "", "", ip::tcp::resolver::flags(), ec);
    (void)results8;

    ip::tcp::resolver::results_type results9 = resolver.resolve(e);
    (void)results9;

    ip::tcp::resolver::results_type results10 = resolver.resolve(e, ec);
    (void)results10;

    resolver.async_resolve("", "", resolve_handler());
    int i1 = resolver.async_resolve("", "", lazy);
    (void)i1;

    resolver.async_resolve("", "",
        ip::tcp::resolver::flags(), resolve_handler());
    int i2 = resolver.async_resolve("", "",
        ip::tcp::resolver::flags(), lazy);
    (void)i2;

    resolver.async_resolve(ip::tcp::v4(), "", "", resolve_handler());
    int i3 = resolver.async_resolve(ip::tcp::v4(), "", "", lazy);
    (void)i3;

    resolver.async_resolve(ip::tcp::v4(),
        "", "", ip::tcp::resolver::flags(), resolve_handler());
    int i4 = resolver.async_resolve(ip::tcp::v4(),
        "", "", ip::tcp::resolver::flags(), lazy);
    (void)i4;

    resolver.async_resolve(e, resolve_handler());
    int i5 = resolver.async_resolve(e, lazy);
    (void)i5;
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_tcp_resolver_compile

//------------------------------------------------------------------------------

// ip_tcp_resolver_entry_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public member functions on the class
// ip::tcp::resolver::entry compile and link correctly. Runtime failures are
// ignored.

namespace ip_tcp_resolver_entry_compile {

void test()
{
  using namespace boost::asio;
  namespace ip = boost::asio::ip;
  const ip::tcp::endpoint endpoint;
  const std::string host_name;
  const std::string service_name;
  const std::allocator<char> alloc;

  try
  {
    // basic_resolver_entry constructors.

    const ip::basic_resolver_entry<ip::tcp> entry1;
    ip::basic_resolver_entry<ip::tcp> entry2(endpoint, host_name, service_name);
    ip::basic_resolver_entry<ip::tcp> entry3(entry1);
    ip::basic_resolver_entry<ip::tcp> entry4(std::move(entry2));

    // basic_resolver_entry functions.

    ip::tcp::endpoint e1 = entry1.endpoint();
    (void)e1;

    ip::tcp::endpoint e2 = entry1;
    (void)e2;

    std::string s1 = entry1.host_name();
    (void)s1;

    std::string s2 = entry1.host_name(alloc);
    (void)s2;

    std::string s3 = entry1.service_name();
    (void)s3;

    std::string s4 = entry1.service_name(alloc);
    (void)s4;
  }
  catch (std::exception&)
  {
  }
}

} // namespace ip_tcp_resolver_entry_compile

//------------------------------------------------------------------------------

// ip_tcp_iostream_compile test
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// The following test checks that all public types and member functions on the
// class ip::tcp::iostream compile and link correctly. Runtime failures are
// ignored.

namespace ip_tcp_iostream_compile {

void test()
{
#if !defined(BOOST_ASIO_NO_IOSTREAM)
  using namespace boost::asio;
  namespace ip = boost::asio::ip;

  boost::asio::io_context ioc;
  boost::asio::ip::tcp::socket sock(ioc);

  // basic_socket_iostream typedefs.

  (void)static_cast<ip::tcp::iostream::protocol_type*>(0);
  (void)static_cast<ip::tcp::iostream::endpoint_type*>(0);
  (void)static_cast<ip::tcp::iostream::clock_type*>(0);
  (void)static_cast<ip::tcp::iostream::time_point*>(0);
  (void)static_cast<ip::tcp::iostream::duration*>(0);
  (void)static_cast<ip::tcp::iostream::traits_type*>(0);

  // basic_socket_iostream constructors.

  ip::tcp::iostream ios1;

  ip::tcp::iostream ios2(std::move(sock));

  ip::tcp::iostream ios3("hostname", "service");

  // basic_socket_iostream operators.

  ios1 = ip::tcp::iostream();

  ios2 = std::move(ios1);

  // basic_socket_iostream members.

  ios1.connect("hostname", "service");

  ios1.close();

  (void)static_cast<std::streambuf*>(ios1.rdbuf());

  basic_socket<ip::tcp>& sref = ios1.socket();
  (void)sref;

  boost::system::error_code ec = ios1.error();
  (void)ec;

  ip::tcp::iostream::time_point tp = ios1.expiry();
  (void)tp;

  ios1.expires_at(tp);

  ip::tcp::iostream::duration d = ip::tcp::iostream::duration();
  ios1.expires_after(d);

  // iostream operators.

  int i = 0;
  ios1 >> i;
  ios1 << i;
#endif // !defined(BOOST_ASIO_NO_IOSTREAM)
}

} // namespace ip_tcp_iostream_compile

//------------------------------------------------------------------------------

BOOST_ASIO_TEST_SUITE
(
  "ip/tcp",
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_compile::test)
  BOOST_ASIO_TEST_CASE(ip_tcp_runtime::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_socket_compile::test)
  BOOST_ASIO_TEST_CASE(ip_tcp_socket_runtime::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_acceptor_compile::test)
  BOOST_ASIO_TEST_CASE(ip_tcp_acceptor_runtime::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_resolver_compile::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_resolver_entry_compile::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_resolver_entry_compile::test)
  BOOST_ASIO_COMPILE_TEST_CASE(ip_tcp_iostream_compile::test)
)

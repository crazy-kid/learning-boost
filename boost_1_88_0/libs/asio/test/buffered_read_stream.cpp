//
// buffered_read_stream.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~
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
#include <boost/asio/buffered_read_stream.hpp>

#include <cstring>
#include <functional>
#include "archetypes/async_result.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/system_error.hpp>
#include "unit_test.hpp"

#if defined(BOOST_ASIO_HAS_BOOST_ARRAY)
# include <boost/array.hpp>
#else // defined(BOOST_ASIO_HAS_BOOST_ARRAY)
# include <array>
#endif // defined(BOOST_ASIO_HAS_BOOST_ARRAY)

typedef boost::asio::buffered_read_stream<
    boost::asio::ip::tcp::socket> stream_type;

void write_some_handler(const boost::system::error_code&, std::size_t)
{
}

void fill_handler(const boost::system::error_code&, std::size_t)
{
}

void read_some_handler(const boost::system::error_code&, std::size_t)
{
}

void test_compile()
{
#if defined(BOOST_ASIO_HAS_BOOST_ARRAY)
  using boost::array;
#else // defined(BOOST_ASIO_HAS_BOOST_ARRAY)
  using std::array;
#endif // defined(BOOST_ASIO_HAS_BOOST_ARRAY)

  using namespace boost::asio;

  try
  {
    io_context ioc;
    char mutable_char_buffer[128] = "";
    const char const_char_buffer[128] = "";
    array<boost::asio::mutable_buffer, 2> mutable_buffers = {{
        boost::asio::buffer(mutable_char_buffer, 10),
        boost::asio::buffer(mutable_char_buffer + 10, 10) }};
    array<boost::asio::const_buffer, 2> const_buffers = {{
        boost::asio::buffer(const_char_buffer, 10),
        boost::asio::buffer(const_char_buffer + 10, 10) }};
    archetypes::lazy_handler lazy;
    boost::system::error_code ec;

    stream_type stream1(ioc);
    stream_type stream2(ioc, 1024);

    stream_type::executor_type ex = stream1.get_executor();
    (void)ex;

    stream_type::lowest_layer_type& lowest_layer = stream1.lowest_layer();
    (void)lowest_layer;

    stream1.write_some(buffer(mutable_char_buffer));
    stream1.write_some(buffer(const_char_buffer));
    stream1.write_some(mutable_buffers);
    stream1.write_some(const_buffers);
    stream1.write_some(null_buffers());
    stream1.write_some(buffer(mutable_char_buffer), ec);
    stream1.write_some(buffer(const_char_buffer), ec);
    stream1.write_some(mutable_buffers, ec);
    stream1.write_some(const_buffers, ec);
    stream1.write_some(null_buffers(), ec);

    stream1.async_write_some(buffer(mutable_char_buffer), &write_some_handler);
    stream1.async_write_some(buffer(const_char_buffer), &write_some_handler);
    stream1.async_write_some(mutable_buffers, &write_some_handler);
    stream1.async_write_some(const_buffers, &write_some_handler);
    stream1.async_write_some(null_buffers(), &write_some_handler);
    int i1 = stream1.async_write_some(buffer(mutable_char_buffer), lazy);
    (void)i1;
    int i2 = stream1.async_write_some(buffer(const_char_buffer), lazy);
    (void)i2;
    int i3 = stream1.async_write_some(mutable_buffers, lazy);
    (void)i3;
    int i4 = stream1.async_write_some(const_buffers, lazy);
    (void)i4;
    int i5 = stream1.async_write_some(null_buffers(), lazy);
    (void)i5;

    stream1.fill();
    stream1.fill(ec);

    stream1.async_fill(&fill_handler);
    int i6 = stream1.async_fill(lazy);
    (void)i6;

    stream1.read_some(buffer(mutable_char_buffer));
    stream1.read_some(mutable_buffers);
    stream1.read_some(null_buffers());
    stream1.read_some(buffer(mutable_char_buffer), ec);
    stream1.read_some(mutable_buffers, ec);
    stream1.read_some(null_buffers(), ec);

    stream1.async_read_some(buffer(mutable_char_buffer), &read_some_handler);
    stream1.async_read_some(mutable_buffers, &read_some_handler);
    stream1.async_read_some(null_buffers(), &read_some_handler);
    int i7 = stream1.async_read_some(buffer(mutable_char_buffer), lazy);
    (void)i7;
    int i8 = stream1.async_read_some(mutable_buffers, lazy);
    (void)i8;
    int i9 = stream1.async_read_some(null_buffers(), lazy);
    (void)i9;
  }
  catch (std::exception&)
  {
  }
}

void test_sync_operations()
{
  using namespace std; // For memcmp.

  boost::asio::io_context io_context;

  boost::asio::ip::tcp::acceptor acceptor(io_context,
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));
  boost::asio::ip::tcp::endpoint server_endpoint = acceptor.local_endpoint();
  server_endpoint.address(boost::asio::ip::address_v4::loopback());

  stream_type client_socket(io_context);
  client_socket.lowest_layer().connect(server_endpoint);

  stream_type server_socket(io_context);
  acceptor.accept(server_socket.lowest_layer());

  const char write_data[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const boost::asio::const_buffer write_buf = boost::asio::buffer(write_data);

  std::size_t bytes_written = 0;
  while (bytes_written < sizeof(write_data))
  {
    bytes_written += client_socket.write_some(
        boost::asio::buffer(write_buf + bytes_written));
  }

  char read_data[sizeof(write_data)];
  const boost::asio::mutable_buffer read_buf = boost::asio::buffer(read_data);

  std::size_t bytes_read = 0;
  while (bytes_read < sizeof(read_data))
  {
    bytes_read += server_socket.read_some(
        boost::asio::buffer(read_buf + bytes_read));
  }

  BOOST_ASIO_CHECK(bytes_written == sizeof(write_data));
  BOOST_ASIO_CHECK(bytes_read == sizeof(read_data));
  BOOST_ASIO_CHECK(memcmp(write_data, read_data, sizeof(write_data)) == 0);

  bytes_written = 0;
  while (bytes_written < sizeof(write_data))
  {
    bytes_written += server_socket.write_some(
        boost::asio::buffer(write_buf + bytes_written));
  }

  bytes_read = 0;
  while (bytes_read < sizeof(read_data))
  {
    bytes_read += client_socket.read_some(
        boost::asio::buffer(read_buf + bytes_read));
  }

  BOOST_ASIO_CHECK(bytes_written == sizeof(write_data));
  BOOST_ASIO_CHECK(bytes_read == sizeof(read_data));
  BOOST_ASIO_CHECK(memcmp(write_data, read_data, sizeof(write_data)) == 0);

  server_socket.close();
  boost::system::error_code error;
  bytes_read = client_socket.read_some(
      boost::asio::buffer(read_buf), error);

  BOOST_ASIO_CHECK(bytes_read == 0);
  BOOST_ASIO_CHECK(error == boost::asio::error::eof);

  client_socket.close(error);
}

void handle_accept(const boost::system::error_code& e)
{
  BOOST_ASIO_CHECK(!e);
}

void handle_write(const boost::system::error_code& e,
    std::size_t bytes_transferred,
    std::size_t* total_bytes_written)
{
  BOOST_ASIO_CHECK(!e);
  if (e)
    throw boost::system::system_error(e); // Terminate test.
  *total_bytes_written += bytes_transferred;
}

void handle_read(const boost::system::error_code& e,
    std::size_t bytes_transferred,
    std::size_t* total_bytes_read)
{
  BOOST_ASIO_CHECK(!e);
  if (e)
    throw boost::system::system_error(e); // Terminate test.
  *total_bytes_read += bytes_transferred;
}

void handle_read_eof(const boost::system::error_code& e,
    std::size_t bytes_transferred)
{
  BOOST_ASIO_CHECK(e == boost::asio::error::eof);
  BOOST_ASIO_CHECK(bytes_transferred == 0);
}

void test_async_operations()
{
  using namespace std; // For memcmp.

  namespace bindns = std;
  using bindns::placeholders::_1;
  using bindns::placeholders::_2;

  boost::asio::io_context io_context;

  boost::asio::ip::tcp::acceptor acceptor(io_context,
      boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 0));
  boost::asio::ip::tcp::endpoint server_endpoint = acceptor.local_endpoint();
  server_endpoint.address(boost::asio::ip::address_v4::loopback());

  stream_type client_socket(io_context);
  client_socket.lowest_layer().connect(server_endpoint);

  stream_type server_socket(io_context);
  acceptor.async_accept(server_socket.lowest_layer(), &handle_accept);
  io_context.run();
  io_context.restart();

  const char write_data[]
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const boost::asio::const_buffer write_buf = boost::asio::buffer(write_data);

  std::size_t bytes_written = 0;
  while (bytes_written < sizeof(write_data))
  {
    client_socket.async_write_some(
        boost::asio::buffer(write_buf + bytes_written),
        bindns::bind(handle_write, _1, _2, &bytes_written));
    io_context.run();
    io_context.restart();
  }

  char read_data[sizeof(write_data)];
  const boost::asio::mutable_buffer read_buf = boost::asio::buffer(read_data);

  std::size_t bytes_read = 0;
  while (bytes_read < sizeof(read_data))
  {
    server_socket.async_read_some(
        boost::asio::buffer(read_buf + bytes_read),
        bindns::bind(handle_read, _1, _2, &bytes_read));
    io_context.run();
    io_context.restart();
  }

  BOOST_ASIO_CHECK(bytes_written == sizeof(write_data));
  BOOST_ASIO_CHECK(bytes_read == sizeof(read_data));
  BOOST_ASIO_CHECK(memcmp(write_data, read_data, sizeof(write_data)) == 0);

  bytes_written = 0;
  while (bytes_written < sizeof(write_data))
  {
    server_socket.async_write_some(
        boost::asio::buffer(write_buf + bytes_written),
        bindns::bind(handle_write, _1, _2, &bytes_written));
    io_context.run();
    io_context.restart();
  }

  bytes_read = 0;
  while (bytes_read < sizeof(read_data))
  {
    client_socket.async_read_some(
        boost::asio::buffer(read_buf + bytes_read),
        bindns::bind(handle_read, _1, _2, &bytes_read));
    io_context.run();
    io_context.restart();
  }

  BOOST_ASIO_CHECK(bytes_written == sizeof(write_data));
  BOOST_ASIO_CHECK(bytes_read == sizeof(read_data));
  BOOST_ASIO_CHECK(memcmp(write_data, read_data, sizeof(write_data)) == 0);

  server_socket.close();
  client_socket.async_read_some(boost::asio::buffer(read_buf), handle_read_eof);
}

BOOST_ASIO_TEST_SUITE
(
  "buffered_read_stream",
  BOOST_ASIO_COMPILE_TEST_CASE(test_compile)
  BOOST_ASIO_TEST_CASE(test_sync_operations)
  BOOST_ASIO_TEST_CASE(test_async_operations)
)

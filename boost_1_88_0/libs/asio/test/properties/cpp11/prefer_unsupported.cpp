//
// cpp11/prefer_unsupported.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2025 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <boost/asio/prefer.hpp>
#include <cassert>

template <int>
struct prop
{
  static constexpr bool is_preferable = true;
};

template <int>
struct object
{
};

namespace boost {
namespace asio {

template<int N, int M>
struct is_applicable_property<object<N>, prop<M> >
{
  static constexpr bool value = true;
};

} // namespace asio
} // namespace boost

int main()
{
  object<1> o1 = {};
  object<1> o2 = boost::asio::prefer(o1, prop<2>());
  object<1> o3 = boost::asio::prefer(o1, prop<2>(), prop<3>());
  object<1> o4 = boost::asio::prefer(o1, prop<2>(), prop<3>(), prop<4>());
  (void)o2;
  (void)o3;
  (void)o4;

  const object<1> o5 = {};
  object<1> o6 = boost::asio::prefer(o5, prop<2>());
  object<1> o7 = boost::asio::prefer(o5, prop<2>(), prop<3>());
  object<1> o8 = boost::asio::prefer(o5, prop<2>(), prop<3>(), prop<4>());
  (void)o6;
  (void)o7;
  (void)o8;

  constexpr object<1> o9 = boost::asio::prefer(object<1>(), prop<2>());
  constexpr object<1> o10 = boost::asio::prefer(object<1>(), prop<2>(), prop<3>());
  constexpr object<1> o11 = boost::asio::prefer(object<1>(), prop<2>(), prop<3>(), prop<4>());
  (void)o9;
  (void)o10;
  (void)o11;
}

# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

cmake_minimum_required(VERSION 3.5...3.16)

project(cmake_install_test LANGUAGES CXX)

find_package(boost_bind REQUIRED)
find_package(boost_core REQUIRED)

add_executable(quick ../quick.cpp)
target_link_libraries(quick Boost::bind Boost::core)

enable_testing()
add_test(quick quick)

add_custom_target(check VERBATIM COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG>)

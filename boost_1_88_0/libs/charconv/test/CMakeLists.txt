# Copyright 2018, 2019 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTestJamfile OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(HAVE_BOOST_TEST)

# https://crascit.com/2015/03/28/enabling-cxx11-in-cmake/
set(CMAKE_CXX_EXTENSIONS OFF)
boost_test_jamfile(FILE Jamfile LINK_LIBRARIES Boost::charconv Boost::core Boost::assert)

endif()

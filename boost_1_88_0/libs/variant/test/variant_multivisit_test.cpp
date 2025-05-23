//-----------------------------------------------------------------------------
// boost-libs variant/test/variant_multivisit_test.cpp source file
// See http://www.boost.org for updates, documentation, and revision history.
//-----------------------------------------------------------------------------
//
// Copyright (c) 2013-2025 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost/config.hpp"
#include "boost/noncopyable.hpp"
#define BOOST_VARAINT_MAX_MULTIVIZITOR_PARAMS 5
#include "boost/variant/multivisitors.hpp"
#include "boost/variant.hpp"

#include "boost/core/lightweight_test.hpp"

struct my_noncopyable : boost::noncopyable {
    my_noncopyable(){}
    ~my_noncopyable(){}
};

typedef boost::variant<my_noncopyable, int>         variant_noncopy_t;


typedef boost::variant<char, unsigned char, signed char, unsigned short, int>         variant5_t;

struct test_visitor: boost::static_visitor<> {
    // operators that shall not be called
    template <class  T1, class  T2, class  T3>
    void operator()(T1&, T2&, T3&) const 
    {
        BOOST_TEST(false);
    }

    template <class  T1, class  T2, class  T3, class  T4>
    void operator()(T1&, T2&, T3&, T4&) const 
    {
        BOOST_TEST(false);
    }

    template <class  T1, class  T2, class  T3, class  T4, class  T5>
    void operator()(T1&, T2&, T3&, T4&, T5&) const 
    {
        BOOST_TEST(false);
    }

    // operators that are OK to call
    void operator()(char v0, unsigned char v1, signed char v2) const 
    {
        BOOST_TEST(v0 == 0);
        BOOST_TEST(v1 == 1);
        BOOST_TEST(v2 == 2);
    }

    void operator()(char v0, unsigned char v1, signed char v2, unsigned short v3) const 
    {
        BOOST_TEST(v0 == 0);
        BOOST_TEST(v1 == 1);
        BOOST_TEST(v2 == 2);
        BOOST_TEST(v3 == 3);
    }

    void operator()(char v0, unsigned char v1, signed char v2, unsigned short v3, int v4) const 
    {
        BOOST_TEST(v0 == 0);
        BOOST_TEST(v1 == 1);
        BOOST_TEST(v2 == 2);
        BOOST_TEST(v3 == 3);
        BOOST_TEST(v4 == 4);
    }


    // Noncopyables
    void operator()(my_noncopyable&, my_noncopyable&, my_noncopyable&) const {
        BOOST_TEST(true);
    }
    void operator()(my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&) const {
        BOOST_TEST(true);
    }
    void operator()(my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&) const {
        BOOST_TEST(true);
    }
    void operator()(my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&, my_noncopyable&) const {
        BOOST_TEST(true);
    }
};

typedef boost::variant<int, double, bool> bool_like_t;
typedef boost::variant<int, double> arithmetics_t;

struct if_visitor: public boost::static_visitor<arithmetics_t> {
    template <class T0, class T1, class T2>
    arithmetics_t operator()(T0 b, T1 v1, T2 v2) const {
        if (!!b) {
            return v1;
        } else {
            return v2;
        }
    }
};


int main()
{
    test_visitor v;

    variant5_t v_array5[5];
    v_array5[0] = char(0);
    v_array5[1] = static_cast<unsigned char>(1);
    v_array5[2] = static_cast<signed char>(2);
    v_array5[3] = static_cast<unsigned short>(3);
    v_array5[4] = static_cast<int>(4);

    boost::apply_visitor(v, v_array5[0], v_array5[1], v_array5[2]);
    boost::apply_visitor(test_visitor(), v_array5[0], v_array5[1], v_array5[2]);

// Following test also pass, but requires many Gigabytes of RAM for compilation and compile for about 15 minutes
//#define BOOST_VARIANT_MULTIVISITORS_TEST_VERY_EXTREME
#ifdef BOOST_VARIANT_MULTIVISITORS_TEST_VERY_EXTREME    
    boost::apply_visitor(v, v_array5[0], v_array5[1], v_array5[2], v_array5[3]);
    boost::apply_visitor(test_visitor(), v_array5[0], v_array5[1], v_array5[2], v_array5[3]);

    boost::apply_visitor(v, v_array5[0], v_array5[1], v_array5[2], v_array5[3], v_array5[4]);
    boost::apply_visitor(test_visitor(), v_array5[0], v_array5[1], v_array5[2], v_array5[3], v_array5[4]);
#endif

    bool_like_t v0(1), v1(true), v2(1.0);

    BOOST_TEST(
        boost::apply_visitor(if_visitor(), v0, v1, v2)
        ==
        arithmetics_t(true)
    );

    if_visitor if_vis;
    BOOST_TEST(
        boost::apply_visitor(if_vis)(v0, v1, v2)
        ==
        arithmetics_t(true)
    );


    variant_noncopy_t vnonc[6];
    boost::apply_visitor(v, vnonc[0], vnonc[1], vnonc[2]);
    boost::apply_visitor(test_visitor(), vnonc[0], vnonc[1], vnonc[2], vnonc[3]);

#ifdef BOOST_VARIANT_MULTIVISITORS_TEST_VERY_EXTREME
    boost::apply_visitor(v, vnonc[0], vnonc[1], vnonc[2], vnonc[3], vnonc[4]);
    boost::apply_visitor(test_visitor(), vnonc[0], vnonc[1], vnonc[2], vnonc[3], vnonc[4], vnonc[5]);
#endif

    return boost::report_errors();
}

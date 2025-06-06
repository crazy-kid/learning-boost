// Boost.Geometry

// Copyright (c) 2018 Yaghyavardhan Singh Khangarot, Hyderabad, India.
// Contributed and/or modified by Yaghyavardhan Singh Khangarot,
//   as part of Google Summer of Code 2018 program.

// This file was modified by Oracle on 2018-2023.
// Modifications copyright (c) 2018-2023 Oracle and/or its affiliates.
// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_DISCRETE_FRECHET_DISTANCE_HPP
#define BOOST_GEOMETRY_ALGORITHMS_DISCRETE_FRECHET_DISTANCE_HPP

#ifdef BOOST_GEOMETRY_DEBUG_FRECHET_DISTANCE
#include <iostream>
#endif

#include <vector>

#include <boost/range/size.hpp>

#include <boost/geometry/algorithms/detail/dummy_geometries.hpp>
#include <boost/geometry/algorithms/detail/throw_on_empty_input.hpp>
#include <boost/geometry/algorithms/not_implemented.hpp>
#include <boost/geometry/core/assert.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/core/point_type.hpp>
#include <boost/geometry/strategies/detail.hpp>
#include <boost/geometry/strategies/discrete_distance/cartesian.hpp>
#include <boost/geometry/strategies/discrete_distance/geographic.hpp>
#include <boost/geometry/strategies/discrete_distance/spherical.hpp>
#include <boost/geometry/strategies/distance_result.hpp>
#include <boost/geometry/util/range.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace discrete_frechet_distance
{

// TODO: The implementation should calculate comparable distances

template <typename SizeType1, typename SizeType2, typename ResultType>
class coup_mat
{
public:
    coup_mat(SizeType1 w, SizeType2 h)
        : m_data(w * h,-1), m_width(w), m_height(h)
    {}

    ResultType & operator()(SizeType1 i, SizeType2 j)
    {
        BOOST_GEOMETRY_ASSERT(i < m_width && j < m_height);
        return m_data[j * m_width + i];
    }

private:
    std::vector<ResultType> m_data;
    SizeType1 m_width;
    SizeType2 m_height;
};

struct linestring_linestring
{
    template <typename Linestring1, typename Linestring2, typename Strategies>
    static inline auto apply(Linestring1 const& ls1, Linestring2 const& ls2,
                             Strategies const& strategies)
    {
        typedef typename distance_result
            <
                point_type_t<Linestring1>,
                point_type_t<Linestring2>,
                Strategies
            >::type result_type;
        typedef typename boost::range_size<Linestring1>::type size_type1;
        typedef typename boost::range_size<Linestring2>::type size_type2;

        boost::geometry::detail::throw_on_empty_input(ls1);
        boost::geometry::detail::throw_on_empty_input(ls2);

        // We can assume the inputs are not empty
        auto const strategy = strategies.distance(dummy_point(), dummy_point());

        size_type1 const a = boost::size(ls1);
        size_type2 const b = boost::size(ls2);

        //Coupling Matrix CoupMat(a,b,-1);
        coup_mat<size_type1, size_type2, result_type> coup_matrix(a, b);

        result_type const not_feasible = -100;
        //findin the Coupling Measure
        for (size_type1 i = 0 ; i < a ; i++ )
        {
            for (size_type2 j = 0 ; j < b ; j++ )
            {
                result_type dis = strategy.apply(range::at(ls1,i), range::at(ls2,j));
                if(i==0 && j==0)
                    coup_matrix(i,j) = dis;
                else if(i==0 && j>0)
                    coup_matrix(i,j) =
                        (std::max)(coup_matrix(i,j-1), dis);
                else if(i>0 && j==0)
                    coup_matrix(i,j) =
                        (std::max)(coup_matrix(i-1,j), dis);
                else if(i>0 && j>0)
                    coup_matrix(i,j) =
                        (std::max)((std::min)(coup_matrix(i,j-1),
                                              (std::min)(coup_matrix(i-1,j),
                                                         coup_matrix(i-1,j-1))),
                                   dis);
                else
                    coup_matrix(i,j) = not_feasible;
            }
        }

        #ifdef BOOST_GEOMETRY_DEBUG_FRECHET_DISTANCE
        //Print CoupLing Matrix
        for(size_type i = 0; i <a; i++)
        {
            for(size_type j = 0; j <b; j++)
            std::cout << coup_matrix(i,j) << " ";
            std::cout << std::endl;
        }
        #endif

        return coup_matrix(a-1,b-1);
    }
};

}} // namespace detail::frechet_distance
#endif // DOXYGEN_NO_DETAIL

#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{
template
<
    typename Geometry1,
    typename Geometry2,
    typename Tag1 = tag_t<Geometry1>,
    typename Tag2 = tag_t<Geometry2>
>
struct discrete_frechet_distance : not_implemented<Tag1, Tag2>
{};

template <typename Linestring1, typename Linestring2>
struct discrete_frechet_distance
    <
        Linestring1,
        Linestring2,
        linestring_tag,
        linestring_tag
    >
    : detail::discrete_frechet_distance::linestring_linestring
{};

} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


namespace resolve_strategy {

template
<
    typename Strategies,
    bool IsUmbrella = strategies::detail::is_umbrella_strategy<Strategies>::value
>
struct discrete_frechet_distance
{
    template <typename Geometry1, typename Geometry2>
    static inline auto apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             Strategies const& strategies)
    {
        return dispatch::discrete_frechet_distance
            <
                Geometry1, Geometry2
            >::apply(geometry1, geometry2, strategies);
    }
};

template <typename Strategy>
struct discrete_frechet_distance<Strategy, false>
{
    template <typename Geometry1, typename Geometry2>
    static inline auto apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             Strategy const& strategy)
    {
        using strategies::discrete_distance::services::strategy_converter;
        return dispatch::discrete_frechet_distance
            <
                Geometry1, Geometry2
            >::apply(geometry1, geometry2,
                     strategy_converter<Strategy>::get(strategy));
    }
};

template <>
struct discrete_frechet_distance<default_strategy, false>
{
    template <typename Geometry1, typename Geometry2>
    static inline auto apply(Geometry1 const& geometry1, Geometry2 const& geometry2,
                             default_strategy const&)
    {
        typedef typename strategies::discrete_distance::services::default_strategy
            <
                Geometry1, Geometry2
            >::type strategies_type;

        return dispatch::discrete_frechet_distance
            <
                Geometry1, Geometry2
            >::apply(geometry1, geometry2, strategies_type());
    }
};

} // namespace resolve_strategy


/*!
\brief \brief_calc2{discrete Frechet distance, between} \brief_strategy
\details \details_free_function{discrete_frechet_distance, discrete Frechet distance, between}.
\ingroup discrete_frechet_distance
\tparam Geometry1 \tparam_geometry
\tparam Geometry2 \tparam_geometry
\tparam Strategy \tparam_strategy{Distance}
\param geometry1 \param_geometry
\param geometry2 \param_geometry
\param strategy \param_strategy{point to point distance}

\qbk{distinguish,with strategy}
\qbk{[include reference/algorithms/discrete_frechet_distance.qbk]}

\qbk{
[heading Available Strategies]
\* [link geometry.reference.strategies.strategy_distance_pythagoras Pythagoras (cartesian)]
\* [link geometry.reference.strategies.strategy_distance_haversine Haversine (spherical)]
\* One of the geographic point to point strategies


[heading Example]
[discrete_frechet_distance_strategy]
[discrete_frechet_distance_strategy_output]
}
*/
template <typename Geometry1, typename Geometry2, typename Strategy>
inline auto discrete_frechet_distance(Geometry1 const& geometry1,
                                      Geometry2 const& geometry2,
                                      Strategy const& strategy)
{
    return resolve_strategy::discrete_frechet_distance
            <
                Strategy
            >::apply(geometry1, geometry2, strategy);
}

// Algorithm overload using default Pt-Pt distance strategy

/*!
\brief \brief_calc2{discrete Frechet distance, between}
\details \details_free_function{discrete_frechet_distance, discrete Frechet distance, between}.
\ingroup discrete_frechet_distance
\tparam Geometry1 \tparam_geometry
\tparam Geometry2 \tparam_geometry
\param geometry1 \param_geometry
\param geometry2 \param_geometry

\qbk{[include reference/algorithms/discrete_frechet_distance.qbk]}

\qbk{
[heading Example]
[discrete_frechet_distance]
[discrete_frechet_distance_output]
}
*/
template <typename Geometry1, typename Geometry2>
inline auto discrete_frechet_distance(Geometry1 const& geometry1,
                                      Geometry2 const& geometry2)
{
    return resolve_strategy::discrete_frechet_distance
            <
                default_strategy
            >::apply(geometry1, geometry2, default_strategy());
}

}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_DISCRETE_FRECHET_DISTANCE_HPP

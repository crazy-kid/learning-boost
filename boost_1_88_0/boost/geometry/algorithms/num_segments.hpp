// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014-2023, Oracle and/or its affiliates.

// Contributed and/or modified by Vissarion Fysikopoulos, on behalf of Oracle
// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

#ifndef BOOST_GEOMETRY_ALGORITHMS_NUM_SEGMENTS_HPP
#define BOOST_GEOMETRY_ALGORITHMS_NUM_SEGMENTS_HPP

#include <cstddef>

#include <boost/range/size.hpp>
#include <boost/range/value_type.hpp>

#include <boost/geometry/algorithms/detail/counting.hpp>
#include <boost/geometry/algorithms/detail/visit.hpp>
#include <boost/geometry/algorithms/not_implemented.hpp>

#include <boost/geometry/core/closure.hpp>
#include <boost/geometry/core/tag.hpp>
#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/core/visit.hpp>

#include <boost/geometry/geometries/adapted/boost_variant.hpp> // For backward compatibility
#include <boost/geometry/geometries/concepts/check.hpp>

namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace num_segments
{


struct range_count
{
    template <typename Range>
    static inline std::size_t apply(Range const& range)
    {
        std::size_t n = boost::size(range);
        if ( n <= 1 )
        {
            return 0;
        }

        return
            geometry::closure<Range>::value == open
            ?
            n
            :
            static_cast<std::size_t>(n - 1);
    }
};

}} // namespace detail::num_segments
#endif // DOXYGEN_NO_DETAIL



#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{

template <typename Geometry, typename Tag = tag_t<Geometry>>
struct num_segments
    : not_implemented<Tag>
{};

template <typename Geometry>
struct num_segments<Geometry, point_tag>
    : detail::counting::other_count<0>
{};

// the number of segments (1-dimensional faces) of the hypercube is
// given by the formula: d * 2^(d-1), where d is the dimension of the
// hypercube; see also:
//            http://en.wikipedia.org/wiki/Hypercube
template <typename Geometry>
struct num_segments<Geometry, box_tag>
    : detail::counting::other_count
        <
            geometry::dimension<Geometry>::value
            * (1 << (geometry::dimension<Geometry>::value - 1))
        >
{};

template <typename Geometry>
struct num_segments<Geometry, segment_tag>
    : detail::counting::other_count<1>
{};

template <typename Geometry>
struct num_segments<Geometry, linestring_tag>
    : detail::num_segments::range_count
{};

template <typename Geometry>
struct num_segments<Geometry, ring_tag>
    : detail::num_segments::range_count
{};

template <typename Geometry>
struct num_segments<Geometry, polygon_tag>
    : detail::counting::polygon_count<detail::num_segments::range_count>
{};

template <typename Geometry>
struct num_segments<Geometry, multi_point_tag>
    : detail::counting::other_count<0>
{};

template <typename Geometry>
struct num_segments<Geometry, multi_linestring_tag>
    : detail::counting::multi_count
        <
            num_segments< typename boost::range_value<Geometry>::type>
        >
{};

template <typename Geometry>
struct num_segments<Geometry, multi_polygon_tag>
    : detail::counting::multi_count
        <
            num_segments< typename boost::range_value<Geometry>::type>
        >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH



namespace resolve_dynamic
{


template <typename Geometry, typename Tag = tag_t<Geometry>>
struct num_segments
{
    static inline std::size_t apply(Geometry const& geometry)
    {
        concepts::check<Geometry const>();

        return dispatch::num_segments<Geometry>::apply(geometry);
    }
};


template <typename Geometry>
struct num_segments<Geometry, dynamic_geometry_tag>
{
    static inline std::size_t apply(Geometry const& geometry)
    {
        std::size_t result = 0;
        traits::visit<Geometry>::apply([&](auto const& g)
        {
            result = num_segments<util::remove_cref_t<decltype(g)>>::apply(g);
        }, geometry);
        return result;
    }
};


template <typename Geometry>
struct num_segments<Geometry, geometry_collection_tag>
{
    static inline std::size_t apply(Geometry const& geometry)
    {
        std::size_t result = 0;
        detail::visit_breadth_first([&](auto const& g)
        {
            result += num_segments<util::remove_cref_t<decltype(g)>>::apply(g);
            return true;
        }, geometry);
        return result;
    }
};


} // namespace resolve_dynamic



/*!
\brief \brief_calc{number of segments}
\ingroup num_segments
\details \details_calc{num_segments, number of segments}.
\tparam Geometry \tparam_geometry
\param geometry \param_geometry
\return \return_calc{number of segments}

\qbk{[include reference/algorithms/num_segments.qbk]}
*/
template <typename Geometry>
inline std::size_t num_segments(Geometry const& geometry)
{
    return resolve_dynamic::num_segments<Geometry>::apply(geometry);
}



}} // namespace boost::geometry

#endif // BOOST_GEOMETRY_ALGORITHMS_NUM_SEGMENTS_HPP

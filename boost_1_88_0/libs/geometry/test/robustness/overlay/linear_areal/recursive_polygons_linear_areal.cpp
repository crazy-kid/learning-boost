// Boost.Geometry (aka GGL, Generic Geometry Library)
// Robustness Test

// Copyright (c) 2012-2015 Barend Gehrels, Amsterdam, the Netherlands.

// This file was modified by Oracle on 2021.
// Modifications copyright (c) 2021, Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if defined(_MSC_VER)
#  pragma warning( disable : 4244 )
#  pragma warning( disable : 4267 )
#endif

#include <chrono>
#include <fstream>
#include <sstream>

#include <boost/program_options.hpp>

#include <boost/geometry/algorithms/append.hpp>
#include <boost/geometry/algorithms/buffer.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/within.hpp>

//#include <boost/geometry/extensions/algorithms/midpoints.hpp>

#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/geometries/point_xy.hpp>

#include <boost/geometry/io/svg/svg_mapper.hpp>

#include <robustness/common/common_settings.hpp>
#include <robustness/common/make_random_generator.hpp>
#include <robustness/common/make_square_polygon.hpp>


namespace bg = boost::geometry;

template <typename Geometry1, typename Geometry2, typename Geometry3>
void create_svg(std::string const& filename
                , Geometry1 const& mp
                , Geometry2 const& ls
                , Geometry3 const& difference
                , Geometry3 const& intersection
                )
{
    typedef typename boost::geometry::point_type<Geometry1>::type point_type;


    std::ofstream svg(filename.c_str());
    boost::geometry::svg_mapper<point_type> mapper(svg, 800, 800);

    boost::geometry::model::box<point_type> box;
    bg::envelope(mp, box);
    bg::buffer(box, box, 1.0);
    mapper.add(box);
    bg::envelope(ls, box);
    bg::buffer(box, box, 1.0);
    mapper.add(box);

    mapper.map(mp, "fill-opacity:0.5;fill:rgb(153,204,0);stroke:rgb(153,204,0);stroke-width:3");
    mapper.map(ls, "stroke-opacity:0.9;stroke:rgb(0,0,0);stroke-width:1");

    mapper.map(intersection,"opacity:0.6;stroke:rgb(0,128,0);stroke-width:5");
    mapper.map(difference,  "opacity:0.6;stroke:rgb(255,0,255);stroke-width:5"); //;stroke-dasharray:1,7;stroke-linecap:round
}


template <typename Linestring, typename Generator, typename Settings>
inline void make_random_linestring(Linestring& line, Generator& generator, Settings const& settings)
{
    using namespace boost::geometry;

    using point_type = point_type_t<Linestring>;
    using coordinate_type = coordinate_type_t<Linestring>;

    coordinate_type x, y;
    x = generator();
    y = generator();

    int count = 3 + generator() % 6;
    int d = 0; // direction

    for (int i = 0; i <= count && x <= settings.field_size; i++, x++, d = 1 - d)
    {
        append(line, make<point_type>(x, y + d));
        append(line, make<point_type>(x, y + 1 - d));
    }

    if (d == 0 && generator() % 4 < 3 && y >= 2)
    {
        d = 1 - d;
        x--;
        y -= 2;
        count = 3 + generator() % 6;
        for (int i = 0; i <= count && x >= 0; i++, x--, d = 1 - d)
        {
            append(line, make<point_type>(x, y + d));
            append(line, make<point_type>(x, y + 1 - d));
        }
    }

    //if (settings.triangular)
    //{
    //    // Remove a point, generator says which
    //    int c = generator() % 4;
    //    if (c >= 1 && c <= 3)
    //    {
    //        ring.erase(ring.begin() + c);
    //    }
    //}
}

template<typename Geometry>
class inside_check
{
    Geometry const& m_geo;
    bool& m_result;
public :

    inside_check(Geometry const& geo, bool& result)
        : m_geo(geo)
        , m_result(result)
    {}

    inline inside_check<Geometry> operator=(inside_check<Geometry> const& input)
    {
        return inside_check<Geometry>(input.m_geo, input.m_result);
    }

    template <typename Point>
    inline void operator()(Point const& p)
    {
        if (! bg::covered_by(p, m_geo))
        {
            if (m_result)
            {
                std::cout << "Does not fulfill inside check" << std::endl;
            }
            m_result = false;
        }
    }
};


template<typename Geometry>
class outside_check
{
    Geometry const& m_geo;
    bool& m_result;
public :
    outside_check(Geometry const& geo, bool& result)
        : m_geo(geo)
        , m_result(result)
    {}

    inline outside_check<Geometry> operator=(outside_check<Geometry> const& input)
    {
        return outside_check<Geometry>(input.m_geo, input.m_result);
    }

    template <typename Point>
    inline void operator()(Point const& p)
    {
        if (bg::within(p, m_geo))
        {
            if (m_result)
            {
                std::cout << "Does not fulfill outside check" << std::endl;
            }
            m_result = false;
        }
    }
};

template<typename Segment>
class border2_check
{
    Segment const& m_segment;
    bool& m_result;

public :
    border2_check(Segment const& seg, bool& result)
        : m_segment(seg)
        , m_result(result)
    {}

    inline border2_check<Segment> operator=(border2_check<Segment> const& input)
    {
        return border2_check<Segment>(input.m_segment, input.m_result);
    }

    template <typename Segment2>
    inline void operator()(Segment2 const& segment)
    {
        using point_t = typename bg::point_type<Segment2>::type ;
        using segment_t = bg::model::segment<point_t>;
        using result_t = bg::segment_intersection_points<point_t> ;
        using policy_t = bg::policies::relate::segments_intersection_points
            <
                result_t
            >;

        // Create copies (TODO: find out why referring_segment does not compile)
        segment_t seg1, seg2;
        bg::convert(m_segment, seg1);
        bg::convert(segment, seg2);

        bg::strategy::intersection::cartesian_segments<> strategy;
        bg::detail::segment_as_subrange<segment_t> subrange1(seg1);
        bg::detail::segment_as_subrange<segment_t> subrange2(seg2);
        bg::segment_intersection_points<point_t> is
            = strategy.apply(subrange1, subrange2, policy_t());

        if (is.count == 2)
        {
            if (m_result)
            {
                std::cout << "Does not fulfill border2 check" << std::endl;
            }
            m_result = true;
        }
    }
};

template<typename Geometry>
class border_check
{
    Geometry const& m_geo;
    bool& m_result;
public :
    border_check(Geometry const& geo, bool& result)
        : m_geo(geo)
        , m_result(result)
    {}

    inline border_check<Geometry> operator=(border_check<Geometry> const& input)
    {
        return border_check<Geometry>(input.m_geo, input.m_result);
    }

    template <typename Segment>
    inline void operator()(Segment const& s)
    {
        bool on_border = false;
        border2_check<Segment> checker(s, on_border);
        bg::for_each_segment(m_geo, checker);

        if (on_border)
        {
            if (m_result)
            {
                std::cout << "Does not fulfill border check" << std::endl;
            }
            m_result = false;
        }
    }
};


template <typename MultiPolygon, typename Linestring, typename Settings>
bool verify(std::string const& caseid, MultiPolygon const& mp, Linestring const& ls, Settings const& settings)
{
    bg::model::multi_linestring<Linestring> difference, intersection;
    bg::difference(ls, mp, difference);
    bg::intersection(ls, mp, intersection);

    //typedef typename bg::length_result_type<point_type>::type length_type;

    bool result = true;

    // 1) Check by length
    typedef double length_type;
    length_type len_input = bg::length(ls);
    length_type len_difference = bg::length(difference);
    length_type len_intersection = bg::length(intersection);
    if (! bg::math::equals(len_input, len_difference + len_intersection))
    {
        std::cout << "Input: " << len_input
            << " difference: " << len_difference
            << " intersection: " << len_intersection
            << std::endl;

        std::cout << "Does not fulfill length check" << std::endl;

        result = false;
    }

    // 2) Check by within and covered by
    inside_check<MultiPolygon> ic(mp, result);
    bg::for_each_point(intersection, ic);

    outside_check<MultiPolygon> oc(mp, result);
    bg::for_each_point(difference, oc);

    border_check<MultiPolygon> bc(mp, result);
    bg::for_each_segment(difference, bc);

    /* commented out since it needs extensions, tests are passing without that part
    // 3) check also the mid-points from the difference to remove false positives
    for (Linestring const& d : difference)
    {
        Linestring difference_midpoints;
        bg::midpoints(d, false, std::back_inserter(difference_midpoints));
        outside_check<MultiPolygon> ocm(mp, result);
        bg::for_each_point(difference_midpoints, ocm);
    }
    */
    if (settings.verbose)
    {
        std::cout << " [" << bg::area(mp) << " " << bg::length(ls) <<
            " " << bg::length(intersection) <<
            " " << bg::length(difference) << "]";
    }

    bool svg = settings.svg;
    bool wkt = settings.wkt;
    if (! result)
    {
        std::cout << "ERROR " << caseid << std::endl;
        std::cout << bg::wkt(mp) << std::endl;
        std::cout << bg::wkt(ls) << std::endl;
        svg = true;
        wkt = true;
    }

    if (svg)
    {
        std::ostringstream filename;
        filename << caseid << "_"
            << typeid(typename bg::coordinate_type<Linestring>::type).name()
            << ".svg";
        create_svg(filename.str(), mp, ls, difference, intersection);
    }

    if (wkt)
    {
        std::ostringstream filename;
        filename << caseid << "_"
            << typeid(typename bg::coordinate_type<Linestring>::type).name()
            << ".wkt";
        std::ofstream stream(filename.str().c_str());
        stream << bg::wkt(mp) << std::endl;
        stream << bg::wkt(ls) << std::endl;
    }

    return result;
}

template <typename MultiPolygon, typename Generator>
bool test_linear_areal(MultiPolygon& result, int& index,
            Generator& generator,
            int level, common_settings const& settings)
{
    MultiPolygon p, q;

    // Generate two boxes
    if (level == 0)
    {
        p.resize(1);
        q.resize(1);
        make_square_polygon(p.front(), generator, settings);
        make_square_polygon(q.front(), generator, settings);
        bg::correct(p);
        bg::correct(q);
    }
    else
    {
        bg::correct(p);
        bg::correct(q);
        if (! test_linear_areal(p, index, generator, level - 1, settings)
            || ! test_linear_areal(q, index, generator, level - 1, settings))
        {
            return false;
        }
    }

    typedef typename boost::range_value<MultiPolygon>::type polygon;

    MultiPolygon mp;
    bg::detail::union_::union_insert
        <
            polygon
        >(p, q, std::back_inserter(mp));

    bg::unique(mp);
    bg::simplify(mp, result, 0.01);
    bg::correct(mp);

    // Generate a linestring
    typedef typename bg::point_type<MultiPolygon>::type point_type;
    typedef bg::model::linestring<point_type> linestring_type;
    linestring_type ls;
    make_random_linestring(ls, generator, settings);

    std::ostringstream out;
    out << "recursive_la_" << index++ << "_" << level;
    return verify(out.str(), mp, ls, settings);
}


template <typename T, bool Clockwise, bool Closed>
void test_all(int seed, int count, int level, common_settings const& settings)
{
    auto coordinate_generator = make_int_generator(seed, settings.field_size - 1);

    typedef bg::model::polygon
        <
            bg::model::d2::point_xy<T>, Clockwise, Closed
        > polygon;
    typedef bg::model::multi_polygon<polygon> mp;

    auto const t0 = std::chrono::high_resolution_clock::now();

    int index = 0;
    for(int i = 0; i < count; i++)
    {
        mp p;
        test_linear_areal<mp>(p, index, coordinate_generator, level, settings);
    }
    auto const t = std::chrono::high_resolution_clock::now();
    auto const elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t - t0).count();
    std::cout << std::endl
        << "geometries: " << index
        << " type: " << typeid(T).name()
        << " time: " << elapsed_ms / 1000.0  << std::endl;
}

int main(int argc, char** argv)
{
    try
    {
        namespace po = boost::program_options;
        po::options_description description("=== recursive_polygons_linear_areal ===\nAllowed options");

        int count = 1;
        int seed = -1;
        int level = 3;
        bool ccw = false;
        bool open = false;
        common_settings settings;
        std::string form = "box";

        description.add_options()
            ("help", "Help message")
            ("seed", po::value<int>(&seed), "Initialization seed for random generator")
            ("count", po::value<int>(&count), "Number of tests")
            ("diff", po::value<bool>(&settings.also_difference), "Include testing on difference")
            ("level", po::value<int>(&level), "Level to reach (higher->slower)")
            ("form", po::value<std::string>(&form)->default_value("box"), "Form of the polygons (box, triangle)")
            ("ccw", po::value<bool>(&ccw), "Counter clockwise polygons")
            ("open", po::value<bool>(&open), "Open polygons")
            ("size", po::value<int>(&settings.field_size), "Size of the field")
            ("verbose", po::value<bool>(&settings.verbose), "Verbose")
            ("wkt", po::value<bool>(&settings.wkt), "Create a WKT of the inputs, for all tests")
            ("svg", po::value<bool>(&settings.svg), "Create a SVG for all tests")
        ;

        po::variables_map varmap;
        po::store(po::parse_command_line(argc, argv, description), varmap);
        po::notify(varmap);

        if (varmap.count("help")
            || (form != "box" && form != "triangle"))
        {
            std::cout << description << std::endl;
            return 1;
        }

        settings.triangular = form != "box";

        if (ccw && open)
        {
            test_all<double, false, false>(seed, count, level, settings);
        }
        else if (ccw)
        {
            test_all<double, false, true>(seed, count, level, settings);
        }
        else if (open)
        {
            test_all<double, true, false>(seed, count, level, settings);
        }
        else
        {
            test_all<double, true, true>(seed, count, level, settings);
        }
    }
    catch(std::exception const& e)
    {
        std::cout << "Exception " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cout << "Other exception" << std::endl;
    }

    return 0;
}

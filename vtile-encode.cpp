// protozero
#include <protozero/varint.hpp>
#include <protozero/pbf_writer.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>


// from mapnik-vector-tile
namespace detail_pbf {

    inline unsigned encode_length(unsigned len)
    {
        return (len << 3u) | 2u;
    }
}

// from mapnik/well_known_srs.hpp
static const double EARTH_RADIUS = 6378137.0;
static const double EARTH_DIAMETER = EARTH_RADIUS * 2.0;
static const double EARTH_CIRCUMFERENCE = EARTH_DIAMETER * M_PI;
static const double MAXEXTENT = EARTH_CIRCUMFERENCE / 2.0;
static const double M_PI_by2 = M_PI / 2;
static const double D2R = M_PI / 180;
static const double R2D = 180 / M_PI;
static const double M_PIby360 = M_PI / 360;
static const double MAXEXTENTby180 = MAXEXTENT / 180;
static const double MAX_LATITUDE = R2D * (2 * std::atan(std::exp(180 * D2R)) - M_PI_by2);

inline void lonlat2merc(double & x, double & y)
{
    if (x > 180) x = 180;
    else if (x < -180) x = -180;
    if (y > MAX_LATITUDE) y = MAX_LATITUDE;
    else if (y < -MAX_LATITUDE) y = -MAX_LATITUDE;
    x = x * MAXEXTENTby180;
    y = std::log(std::tan((90 + y) * M_PIby360)) * R2D;
    y = y * MAXEXTENTby180;
}

// emulates mapbox::box2d
class bbox {
 public:
    double minx;
    double miny;
    double maxx;
    double maxy;
    bbox(double _minx,double _miny,double _maxx,double _maxy) :
      minx(_minx),
      miny(_miny),
      maxx(_maxx),
      maxy(_maxy) { }

    double width() const {
        return maxx - minx;
    }

    double height() const {
        return maxy - miny;
    }
};

// should start using core geometry class across mapnik, osrm, mapbox-gl-native
class point_type_d {
 public:
    double x;
    double y;
    point_type_d(double _x, double _y) :
      x(_x),
      y(_y) {

    }
};

class point_type_i {
 public:
    std::int64_t x;
    std::int64_t y;
    point_type_i(std::int64_t _x, std::int64_t _y) :
      x(_x),
      y(_y) {

    }
};

using line_type = std::vector<point_type_i>;
using line_typed = std::vector<point_type_d>;

// from mapnik-vector-tile
inline bool encode_linestring(line_type line,
                       protozero::packed_field_uint32 & geometry,
                       int32_t & start_x,
                       int32_t & start_y) {
    std::size_t line_size = line.size();
    //line_size -= detail_pbf::repeated_point_count(line);
    if (line_size < 2)
    {
        return false;
    }

    unsigned line_to_length = static_cast<unsigned>(line_size) - 1;

    auto pt = line.begin();
    geometry.add_element(9); // move_to | (1 << 3)
    geometry.add_element(protozero::encode_zigzag32(pt->x - start_x));
    geometry.add_element(protozero::encode_zigzag32(pt->y - start_y));
    start_x = pt->x;
    start_y = pt->y;
    geometry.add_element(detail_pbf::encode_length(line_to_length));
    for (++pt; pt != line.end(); ++pt)
    {
        int32_t dx = pt->x - start_x;
        int32_t dy = pt->y - start_y;
        /*if (dx == 0 && dy == 0)
        {
            continue;
        }*/
        geometry.add_element(protozero::encode_zigzag32(dx));
        geometry.add_element(protozero::encode_zigzag32(dy));
        start_x = pt->x;
        start_y = pt->y;
    }
    return true;
}

int main() {

    unsigned tile_extent = 4096;
    // line in geographical coords (doubles)
    line_typed geo_line;
    geo_line.emplace_back(-137.109375,56.36525013685606);
    geo_line.emplace_back(-127.61718749999999,51.83577752045248);
    geo_line.emplace_back(-118.47656249999999,40.979898069620155);
    geo_line.emplace_back(-118.47656249999999,28.92163128242129);
    geo_line.emplace_back(-119.17968749999999,19.642587534013046);

    // 0/0/0 in mercator
    bbox tile_bbox(-20037508.342789,-20037508.342789,20037508.342789,20037508.342789);

    line_type tile_line;
    for (auto const pt : geo_line) {
        // convert from wgs84 -> mercator
        double px_merc = pt.x;
        double py_merc = pt.y;
        lonlat2merc(px_merc,py_merc);
        // convert to integer tile coordinates
        std::int64_t px = std::round((px_merc - tile_bbox.minx) * tile_extent/16 / tile_bbox.width());
        std::int64_t py = std::round((tile_bbox.maxy - py_merc) * tile_extent/16 / tile_bbox.width());
        tile_line.emplace_back(px*tile_extent/256,py*tile_extent/256);
    }

    // TODO - lines need to be clipped to the tile boundaries

    // set up for delta encoding
    std::int32_t start_x = 0;
    std::int32_t start_y = 0;

    std::string buffer;
    protozero::pbf_writer tile_writer(buffer);
    {
        protozero::pbf_writer layer_writer(tile_writer,3);
        layer_writer.add_uint32(15,2); // version
        layer_writer.add_string(1,"layer-name"); // name
        layer_writer.add_uint32(5,4096); // name
        {
            protozero::pbf_writer feature_writer(layer_writer,2);
            feature_writer.add_enum(3,2); // geometry type
            feature_writer.add_uint64(1,1); // id
            {
                protozero::packed_field_uint32 geometry(feature_writer,4);
                encode_linestring(tile_line,geometry,start_x,start_y);
            }
        }
    }

    std::cout << buffer;
}
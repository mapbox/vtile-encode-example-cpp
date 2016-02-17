{
  "includes": [
      "common.gypi"
  ],
  'variables': {
    'common_defines' : [
        'CLIPPER_INTPOINT_IMPL=mapnik::geometry::point<cInt>',
        'CLIPPER_PATH_IMPL=mapnik::geometry::line_string<cInt>',
        'CLIPPER_PATHS_IMPL=mapnik::geometry::multi_line_string<cInt>',
        'CLIPPER_IMPL_INCLUDE=<mapnik/geometry.hpp>'
    ]
  },
  "targets": [
    {
      "target_name": "vtile-encode",
      "type": "executable",
      "defines": [
        "<@(common_defines)"
      ],
      "sources": [
        "../vtile-encode.cpp"
      ],
      "include_dirs": [
        "../src",
        '../deps/protozero/include'
      ]
    }
  ]
}

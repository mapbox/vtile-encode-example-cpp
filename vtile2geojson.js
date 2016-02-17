#!/usr/bin/env node

"use strict";

var mapnik = require('mapnik');

var input = [];

process.stdin.on('readable', function() {
  var chunk = process.stdin.read();
  if (chunk !== null) {
    input.push(chunk);
  }
});

process.stdin.on('end', function() {
    for (var length = 0, i = 0; i < input.length; i++) {
        length += input[i].length;
    }
    var result = new Buffer(length);
    for (var pos = 0, j = 0; j < input.length; j++) {
        input[j].copy(result, pos);
        pos += input[j].length;
    }

    var vt = new mapnik.VectorTile(0,0,0);
    vt.setData(result);
    console.log(vt.toGeoJSON('__all__'));
});

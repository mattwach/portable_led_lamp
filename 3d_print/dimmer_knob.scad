use <lib/mattwach/util.scad>
use <lib/mattwach/shapes.scad>

module dimmer_knob() {
  overlap = 0.01;

  hole_diameter = 6;
  hole_depth = 7;

  height = 9;
  diameter = 15;

  difference() {
    cylinder(d=diameter, h=height);
    tz(-overlap) rounded_cylinder(d=hole_diameter, h=hole_depth + overlap, chamfer=-1);
  }
}

$fa = 2;
$fs = 0.5;
dimmer_knob();

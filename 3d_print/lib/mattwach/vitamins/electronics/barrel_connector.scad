use <../../util.scad>

module barrel_mount_female_5_5(add_hex_nut=true, hex_nut_gap=2) {
  base_diameter = 7.1;
  base_height = 8;
  hex_nut_height = 2;
  hole_diameter = 5.6;
  hole_offset = 1;
  top_height = 3.3;

  overlap = 0.01;

  pin_outer_span = 5.8;
  pin_thickness = 0.6;

  module base() {
    module threads() {
      thread_diameter = 7.8;
      thread_height = 5;
      thread_count = 5;

      start_z = base_height - thread_height;
      thread_size = thread_height / thread_count / 2;
      for (i = [0:thread_count - 1]) {
        tz(start_z + i * thread_size * 2) cylinder(d=thread_diameter, h=thread_size);
      }
    }

    module flats() {
      flat_width = 6.9;
      module cutout() {
        translate([
            -base_diameter / 2,
            0,
            -overlap
        ]) cube([base_diameter, 1, base_height + overlap * 2]);
      }
      ty(flat_width / 2) cutout();
      ty(-flat_width / 2 - 1) cutout();
    }

    module top() {
      top_diameter = 10.1;
      tz(base_height - overlap) cylinder(d=top_diameter, h=top_height + overlap);
    }

    module hole() {
      tz(hole_offset) cylinder(
          d=hole_diameter, h=base_height + top_height - hole_offset + overlap);
    }

    color("#222") difference() {
      union() {
        difference() {
          union() {
            cylinder(d=base_diameter, h=base_height);
            threads();
          }
          flats();
        }
        top();
      }
      hole();
    }
  }

  module rim() {
    rim_outer_diameter = 9.2;
    rim_height = 0.6;
    tz(base_height + top_height) color("#bbb") difference() {
      cylinder(d=rim_outer_diameter, h=rim_height);
      tz(-overlap) cylinder(d=hole_diameter, h=rim_height + overlap * 2);
    }
  }

  module inner_pin() {
    inner_pin_diameter = 1.8;
    inner_pin_inset = 2.8;
    color("#bbb") tz(hole_offset - overlap) cylinder(
        d=inner_pin_diameter,
        h=base_height + top_height - inner_pin_inset + overlap);
  }

  module pin(height) {
    pin_width = 2.2;
    pin_hole_diameter = 1.2;
    color("#bbb") difference() {
      union() {
        translate([
            -pin_thickness / 2,
            -pin_width / 2,
            -height + pin_width / 2,
        ]) cube([pin_thickness, pin_width, height - pin_width / 2]);
        translate([
            -pin_thickness / 2,
            0,
            -height + pin_width / 2
        ]) ry(90) cylinder(d=pin_width, h=pin_thickness);
      }
      translate([
            -pin_thickness / 2 - overlap,
            0,
            -height + pin_width / 2
      ]) ry(90) cylinder(d=pin_hole_diameter, h=pin_thickness + overlap * 2);
    }
  }

  module hex_nut() {
    hex_nut_flat_size = 11;
    hex_nut_diameter = hex_nut_flat_size / cos(360 / 12);
    color("#ddd") difference() {
      cylinder(d=hex_nut_diameter, h=hex_nut_height, $fn=6);
      tz(-overlap) cylinder(d=base_diameter, h=hex_nut_height + overlap * 2);
    }
  }

  tz(-base_height) {
    base();
    rim();
    inner_pin();
    tx(-pin_outer_span / 2 + pin_thickness / 2) pin(5.8);
    tx(pin_outer_span / 2 - pin_thickness / 2) pin(4.6);
    if (add_hex_nut) {
      tz(base_height - hex_nut_height - hex_nut_gap) hex_nut();
    }
  }
}

/*
$fa=2.0;
$fs=0.5;
barrel_mount_female_5_5();
*/


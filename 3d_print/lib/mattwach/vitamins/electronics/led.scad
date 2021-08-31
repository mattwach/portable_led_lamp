use <../../util.scad>

module red_green_3mm_led(
    green_pin_length = 3,
    hover=1.5) {
  red_pin_length = green_pin_length + 1;
  black_pin_length = red_pin_length + 1;
  pin_spacing = 2.54;
  pin_diameter = 0.5;
  overlap = 0.1;
  
  module pin(length) {
    translate([-pin_diameter / 2, -pin_diameter / 2, -length + hover + overlap]) cube([pin_diameter, pin_diameter, length]);
  }

  module pin_connector() {
    translate([-pin_diameter / 2, pin_diameter / 2 - overlap, hover - pin_diameter * 2])
    cube([pin_diameter, pin_spacing - pin_diameter + overlap * 2, pin_diameter * 2]);
  }

  module bulb() {
    height = 5.4;
    diameter = 3;
    color("#ddd", 0.5) translate([0, pin_spacing, hover]) union() {
      translate([0, 0, height - diameter / 2]) sphere(d=diameter);
      cylinder(d=diameter, h=height - diameter / 2);
    }
  }

  bulb();
  color("#faa") union() {
    pin(red_pin_length);
    pin_connector();
  }
  color("#aaa") translate([0, pin_spacing, 0]) pin(black_pin_length);
  color("#afa") union() {
    translate([0, pin_spacing * 2, 0]) pin(green_pin_length);
    translate([0, pin_spacing, 0]) pin_connector();
  }
}

module hex_led_3w() {
  overlap = 0.01;
  base_thickness = 1.3;
  bottom_tin_thickness = 0.1;
  module base(thickness, base_color) {
    // length across the hex flats
    flat_length = 19.7;
    hex_radius = flat_length / 2 / cos(30);

    module mount_holes() {
      module mount_hole() {
        mount_hole_diameter = 4.5;
        mount_hole_inset = mount_hole_diameter / 5;
        tx(hex_radius - mount_hole_inset) tz(-overlap)
          cylinder(d=mount_hole_diameter, h=thickness + overlap * 2);
      }

      for (i=[0:5]) {
        rz(i * 60) mount_hole();
      }
    }

    color(base_color) rz(90) difference() {
      cylinder(r=hex_radius, h=thickness, $fn=6);
      mount_holes();
    }
  }

  module pads() {
    module pad() {
      pad_xsize = 2.5;
      pad_ysize = 4.3;
      pad_height = 0.1;
      color("#ccc") translate([
        6,
        -pad_ysize / 2,
        base_thickness
      ]) cube([pad_xsize, pad_ysize, pad_height]);
    }

    for (i=[0:5]) {
      rz(i * 60) pad();
    }
  }

  module led() {
    led_base_diameter = 7.9;
    led_base_height = 2.6;
    module led_base() {
      color("#eee") tz(base_thickness) cylinder(d=led_base_diameter, h=led_base_height);
    }

    module led_pin() {
      led_pin_width = 1.2;
      led_pin_height = 1.4;
      led_pin_length = 3;
      color("#ccc") tx(led_base_diameter / 2 - 1) tz(base_thickness)
        cube([led_pin_length + 1, led_pin_width, led_pin_height]);
    }

    module led_core() {
      led_core_diameter = 4;
      led_core_height = 0.8;
      color("#ff0") tz(base_thickness + led_base_height)
        cylinder(d=led_core_diameter, h=led_core_height);
    }

    module led_dome() {
      led_dome_diameter = 5.1;
      color("#fff", 0.2) tz(base_thickness + led_base_height) sphere(d=led_dome_diameter);
    }

    led_base();
    led_pin();
    rz(180) led_pin();
    led_core();
    led_dome();
  }

  tz(bottom_tin_thickness) base(base_thickness - bottom_tin_thickness, "#222");
  base(bottom_tin_thickness + overlap, "#ccc");
  pads();
  led();
}

/*
$fa=2;
$fs=0.5;
hex_led_3w();
*/

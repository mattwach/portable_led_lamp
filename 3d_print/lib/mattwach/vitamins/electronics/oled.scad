use <../../util.scad>
include <../../../NopSCADlib/utils/core/core.scad>
include <../../../NopSCADlib/vitamins/pin_headers.scad>

module oled_128x64_i2c(include_pin_headers=true) {
  overlap = 0.01;

  bolt_ring_extrude = 0.1;
  cable_slot_depth = 1.5;
  pcb_length = 27.5;
  pcb_thickness= 1.2;
  pcb_width = 27.8;
  pin_hole_top_offset = 1.6;

  module pcb() {

    mount_hole_xspan = 23.5;
    mount_hole_yspan = 23.7;
    pin_hole_spacing = 2.54;

    module mount_hole() {
      mount_hole_diameter = 2;
      tz(-overlap - bolt_ring_extrude) cylinder(
          d=mount_hole_diameter, h=pcb_thickness + bolt_ring_extrude * 2 + overlap * 2);
    }

    module pin_hole() {
      pin_hole_diameter = 0.7;
      translate([
          pcb_length / 2,
          pcb_width - pin_hole_top_offset,
          -overlap
      ]) cylinder(d=pin_hole_diameter, h=pcb_thickness + overlap * 2);
    }

    module cable_slot() {
      cable_slot_length = 14;
      translate([
          (pcb_length - cable_slot_length) / 2,
          -overlap,
          -overlap
      ]) cube([
          cable_slot_length,
          cable_slot_depth + overlap,
          pcb_thickness + overlap * 2]);
    }

    module bolt_ring() {
      bolt_ring_diameter = 4;
      color("#ddd") tz(-bolt_ring_extrude) cylinder(
          d=bolt_ring_diameter, h=pcb_thickness + bolt_ring_extrude * 2); 
    }

    module board_with_bolt_rings() {
      difference() {
        color("#008") cube([
            pcb_length,
            pcb_width,
            pcb_thickness]);
        cable_slot();
      }
      txy((pcb_length - mount_hole_xspan) / 2,
          (pcb_width - mount_hole_yspan) / 2) bolt_ring();  
      txy((pcb_length - mount_hole_xspan) / 2 + mount_hole_xspan,
          (pcb_width - mount_hole_yspan) / 2) bolt_ring();  
      txy((pcb_length - mount_hole_xspan) / 2 + mount_hole_xspan,
          (pcb_width - mount_hole_yspan) / 2 + mount_hole_yspan) bolt_ring();
      txy((pcb_length - mount_hole_xspan) / 2,
          (pcb_width - mount_hole_yspan) / 2 + mount_hole_yspan) bolt_ring();
    }

    difference() {
      board_with_bolt_rings();
      txy((pcb_length - mount_hole_xspan) / 2,
          (pcb_width - mount_hole_yspan) / 2) mount_hole();  
      txy((pcb_length - mount_hole_xspan) / 2 + mount_hole_xspan,
          (pcb_width - mount_hole_yspan) / 2) mount_hole();  
      txy((pcb_length - mount_hole_xspan) / 2 + mount_hole_xspan,
          (pcb_width - mount_hole_yspan) / 2 + mount_hole_yspan) mount_hole();
      txy((pcb_length - mount_hole_xspan) / 2,
          (pcb_width - mount_hole_yspan) / 2 + mount_hole_yspan) mount_hole();
      tx(-pin_hole_spacing * 3 / 2) pin_hole();
      tx(-pin_hole_spacing / 2) pin_hole();
      tx(pin_hole_spacing / 2) pin_hole();
      tx(pin_hole_spacing * 3 / 2) pin_hole();
    }
  }

  module screen() {
    screen_top_offset = 4.7;
    oled_height = 15.5;
    oled_thickness = 0.7;

    module oled() {
      color("#222") translate([
          0,
          pcb_width - oled_height - screen_top_offset,
          pcb_thickness
      ]) cube([
        pcb_width,
        oled_height,
        oled_thickness
      ]);
    }

    module glass_cover() {
      glass_cover_height = 18.6;
      glass_cover_thickness = 2.8 - oled_thickness - pcb_thickness;
      color("#ddd", 0.1) translate([
          0,
          pcb_width - glass_cover_height - screen_top_offset,
          pcb_thickness + oled_thickness
      ]) cube([
        pcb_width,
        glass_cover_height,
        glass_cover_thickness
      ]);
    }

    module ribbon_cable() {
      ribbon_cable_width = 11;
      color("#322") union() {
        translate([
            (pcb_width - ribbon_cable_width) / 2,
            cable_slot_depth - overlap,
            pcb_thickness
        ]) cube([
          ribbon_cable_width,
          pcb_width - oled_height - screen_top_offset - cable_slot_depth,
          oled_thickness
        ]);
        translate([
            (pcb_width - ribbon_cable_width) / 2,
            cable_slot_depth - oled_thickness,
            0
        ]) cube([
          ribbon_cable_width,
          pcb_width - oled_height - screen_top_offset - cable_slot_depth,
          oled_thickness + pcb_thickness
        ]);
      }
    }

    oled();
    glass_cover();
    ribbon_cable();
  }

  module pin_headers() {
    translate([
        pcb_length / 2,
        pcb_width - pin_hole_top_offset,
        0
    ]) ry(180) pin_header(2p54header, 4, 1);
  }

  pcb();
  screen();
  if (include_pin_headers) {
    pin_headers();
  }
}

/*
$fa=2;
$fs=0.5;
oled_128x64_i2c();
*/


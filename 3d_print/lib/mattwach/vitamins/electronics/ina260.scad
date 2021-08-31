// Adafruit INA260 Breakout Board
use <../../util.scad>

include <../../../NopSCADlib/utils/core/core.scad>
include <../../../NopSCADlib/vitamins/pin_headers.scad>

module ina260(add_pin_header = true) {
  overlap = 0.01;

  pcb_xsize = 22.9;
  pcb_ysize = 23.1;
  pcb_thickness = 1.6;

  hole_yoffset = 2.3;
  hole_xoffset = (pcb_xsize - (7 * 2.54)) / 2;
  hole_diameter = 0.7;
  pad_thickness = 0.1;
  pad_pad = 0.5;

  mounting_hole_diameter = 3;
  mounting_hole_side_pad = 1.5;

  power_pad_xspacing = 5.6;
  power_pad_ysize = 4.8;
  power_pad_yoffset = 2.3;

  module pcb() {
    pcb_fillet = 3;
    module corner() { 
      cylinder(r=pcb_fillet, h=pcb_thickness);
    }
    color("#009") hull() {
      txy(pcb_fillet, pcb_fillet) corner();
      txy(pcb_xsize - pcb_fillet, pcb_fillet) corner();
      txy(pcb_xsize - pcb_fillet, pcb_ysize - pcb_fillet) corner();
      txy(pcb_fillet, pcb_ysize - pcb_fillet) corner();
    }

    module pin_hole_pad() {
      color("#ddd") translate([
          hole_xoffset,
          hole_yoffset,
          -pad_thickness
      ]) cylinder(d=hole_diameter + pad_pad * 2, h=pcb_thickness + pad_thickness * 2);
    }

    for (i=[0:7]) {
      tx(i * 2.54) pin_hole_pad();
    }

    module mounting_hole_pad() {
      color("#ec6") translate([
          0,
          pcb_ysize - mounting_hole_diameter / 2 - mounting_hole_side_pad,
          -pad_thickness,
      ]) cylinder(d=mounting_hole_diameter + pad_pad * 2, h=pcb_thickness + pad_thickness * 2);
    }

    tx(mounting_hole_diameter / 2 + mounting_hole_side_pad) mounting_hole_pad();
    tx(pcb_xsize - mounting_hole_diameter / 2 - mounting_hole_side_pad) mounting_hole_pad();
 
    module power_pads() {
      module power_pad() {
        power_pad_xsize = 2.4;
        module edge() {
          color("#ddd") translate([
              pcb_xsize / 2,
              pcb_ysize - power_pad_yoffset - power_pad_xsize / 2,
              -pad_thickness
          ]) cylinder(d=power_pad_xsize, h=pcb_thickness + pad_thickness * 2);
        }

        color("#ddd") hull() {
          edge();
          ty(-power_pad_ysize + power_pad_xsize) edge();
        }
      }

    tx(-power_pad_xspacing / 2) power_pad();
    tx(power_pad_xspacing / 2) power_pad();
  }


    power_pads();
  }

  module pin_holes() {
    module hole() {
      translate([
          hole_xoffset,
          hole_yoffset,
          -overlap - pcb_thickness
      ]) cylinder(
        d=hole_diameter,
        h=pcb_thickness + pcb_thickness * 2 + overlap * 2);
    }

    for (i=[0:7]) {
      tx(i * 2.54) hole();
    }
  }

  module pins() {
    txy(pcb_xsize / 2, hole_yoffset) ry(180) pin_header(2p54header, 8, 1);
  }

  module mounting_holes() {
    module mounting_hole() {
      translate([
          0,
          pcb_ysize - mounting_hole_diameter / 2 - mounting_hole_side_pad,
          -overlap - pad_thickness,
      ]) cylinder(
        d=mounting_hole_diameter,
        h=pcb_thickness + pad_thickness * 2 + overlap * 2);
    }

    tx(mounting_hole_diameter / 2 + mounting_hole_side_pad) mounting_hole();
    tx(pcb_xsize - mounting_hole_diameter / 2 - mounting_hole_side_pad) mounting_hole();
  }

  module chip() {
    chip_xsize = 4.4;
    chip_ysize = 5.1;
    chip_thickness = 0.8;
    chip_xoffset = (pcb_xsize - chip_xsize) / 2;
    chip_yoffset = 7.7;
    pin_length = 1.5;

    module chip_pins() {
      module pin_row() {
        pin_width = 0.2;
        module chip_pin() {
          pin_thickness = 0.4;
          color("#ddd") translate([
              chip_xoffset - pin_length,
              chip_yoffset,
              pcb_thickness
          ]) cube([pin_length, pin_width, pin_thickness]);
        }

        gap = (chip_ysize - pin_width) / 7;

        for (i = [0:7]) {
          ty(i * gap) chip_pin();
        }
      }

      pin_row();
      tx(chip_xsize + pin_length) pin_row();
    }

    color("#222") translate([
        chip_xoffset,
        chip_yoffset,
        pcb_thickness]) cube([chip_xsize, chip_ysize, chip_thickness]);

    chip_pins();
  }

  module power_pad_holes() {
    module power_pad_hole() {
      power_pad_hole_diameter = 1.2;
      translate([
          pcb_xsize / 2,
          pcb_ysize - power_pad_yoffset - power_pad_ysize / 2,
          -pad_thickness - overlap
      ]) cylinder(
        d=power_pad_hole_diameter,
        h=pcb_thickness + pad_thickness * 2 + overlap * 2);
    }

    tx(-power_pad_xspacing / 2) power_pad_hole();
    tx(power_pad_xspacing / 2) power_pad_hole();
  }

  difference() {
    pcb();
    pin_holes();
    mounting_holes();
    power_pad_holes();
  }
  chip();
  if (add_pin_header) {
    pins();
  }
}

$fa=2;
$fs=0.5;
ina260();

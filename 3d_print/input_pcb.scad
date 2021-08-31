use <lib/mattwach/util.scad>
use <lib/mattwach/vitamins/electronics/buttons.scad>
use <lib/mattwach/vitamins/electronics/oled.scad>
use <lib/mattwach/vitamins/electronics/potentiometers.scad>

include <lib/NopSCADlib/core.scad>
include <lib/NopSCADlib/vitamins/pcbs.scad>
include <lib/NopSCADlib/vitamins/pin_headers.scad>

module input_pcb() {
  board_x_offset = 5.85;
  board_y_offset = 3.5;
  board_thickness = 1.6;
  pin_spacing = 2.54;

  function cx(x) = board_x_offset + x * pin_spacing;
  function cy(y) = board_y_offset + y * pin_spacing;

  module base_pcb() {
    txy(35, 15) pcb(PERF70x30);
  }

  module green_button() {
    translate([cx(18), cy(8), board_thickness])
      rz(-90) push_switch_12x12(button_color="#0D0");
  }

  module red_button() {
    translate([cx(18), cy(2), board_thickness])
      rz(-90) push_switch_12x12();
  }

  module oled_socket() {
    translate([cx(11.5), cy(9), board_thickness]) pin_socket(2p54header, 4, 1);
  }

  module oled_support_socket() {
    translate([cx(11.5), cy(0), board_thickness]) pin_socket(2p54header, 10, 1);
  }

  module pot() {
    translate([
        12,
        15.5,
        board_thickness
    ]) rz(-90) potentiometer_plate_mount();
  }

  module oled() {
    translate([
        21.3,
        0.1,
        12.7,
    ]) oled_128x64_i2c();
  }

  module connector() {
    translate([
        70,
        14.9,
        -0.6]) ry(90) rz(-90) jst_xh_header(jst_xh_header, 8);
  }

  base_pcb();
  green_button();
  red_button();
  oled_socket();
  oled_support_socket();
  pot();
  oled();
  connector();
}

$fa = 2;
$fs = 0.5;
input_pcb();


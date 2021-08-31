use <lib/mattwach/util.scad>
use <lib/mattwach/vitamins/electronics/arduino.scad>
use <lib/mattwach/vitamins/electronics/capacitor.scad>
use <lib/mattwach/vitamins/electronics/ina260.scad>
use <lib/mattwach/vitamins/electronics/voltage_regulators.scad>
use <lib/mattwach/vitamins/rc/jst.scad>

include <lib/NopSCADlib/core.scad>
include <lib/NopSCADlib/vitamins/axial.scad>
include <lib/NopSCADlib/vitamins/components.scad>
include <lib/NopSCADlib/vitamins/pcbs.scad>
include <lib/NopSCADlib/vitamins/pin_headers.scad>

module master_pcb() {
  board_x_offset = 5.85;
  board_y_offset = 3.5;
  board_thickness = 1.6;
  pin_spacing = 2.54;

  function cx(x) = board_x_offset + x * pin_spacing;
  function cy(y) = board_y_offset + y * pin_spacing;

  module base_pcb() {
    txy(35, 25) pcb(PERF70x50);
  }

  module arduino_sockets() {
    module socket_row() {
      tx(cx(5.5)) tz(board_thickness) pin_socket(2p54header, 12, 1);
    }

    ty(cy(1)) socket_row();
    ty(cy(7)) socket_row();
  }

  module ina260_socket() {
    translate([
      cx(3.5),
      cy(10),
      board_thickness
    ]) pin_socket(2p54header, 8, 1);
  }

  module input_connector() {
    translate([
      cx(18.5),
      cy(1),
      board_thickness
    ]) jst_xh_header(jst_xh_header, 8);
  }

  module resistors() {
    module resistor(ohms) {
      tz(board_thickness) ax_res(res1_4, ohms);
    }

    txy(cx(20.5), cy(7)) resistor(47000);
    txy(cx(20.5), cy(9)) resistor(1000);
    txy(cx(14.5), cy(11)) resistor(10000);
    txy(cx(22), cy(14.5)) rz(90) resistor(47000);
    txy(cx(13), cy(14.5)) rz(90) resistor(47000);
  }

  module npn_transistor() {
    txy(cx(22.5), cy(12)) rz(90) small_vreg(pin_length = 5, hover = 4);
  }

  module led_headers() {
    translate([
      cx(18),
      cy(15),
      board_thickness
    ]) pin_header(2p54header, 3, 3);
  }

  module i2c_connector() {
    translate([
      cx(12.5),
      cy(9),
      board_thickness
    ]) pin_header(2p54header, 2, 1);
  }

  module capacitors() {
    txy(cx(10), cy(10)) rz(90) electrolytic_capacitor(
        diameter=5.1,
        height=9,
        pin_spacing=2.54,
        label_text="10u",
        pin_length=3,
        hover=board_thickness);


    translate([cx(11), cy(14), board_thickness]) rz(90)
      ceramic_capacitor("100n");
  }

  module voltage_regulator() {
    txy(cx(10), cy(12)) rz(90) small_vreg(pin_length = 5, hover = 4);
  }

  module power_connector() {
    translate([
      cx(10.5),
      cy(16),
      board_thickness
    ]) jst_xh_header(jst_xh_header, 2);
  }

  module pfet() {
    translate([
        42,
        41.6,
        18]) rz(90) rx(90) TO220("PFEt", lead_length=7);
  }

  module arduino() {
    translate([
      1.8,
      22.9,
      12.7
    ]) rz(-90) arduino_pro_mini(rotated_sda_scl_pins=true);
  }

  module ina260_board() {
    translate([
        cx(-1),
        cy(9),
        12.7
    ]) ina260();
  }

  module i2c_cable() {
    module jst() {
      rz(90) ry(90) jst_male();
    }

    translate([
      cx(4.5),
      cy(6),
      29.5
    ]) jst();

    translate([
      cx(13),
      cy(9),
      17.3
    ]) jst();
  }

  module power_jst() {
    translate([5, 40, 20]) rz(-90) ry(180) jst_female();
  }

  base_pcb();
  arduino_sockets();
  ina260_socket();
  input_connector();
  resistors();
  npn_transistor();
  led_headers();
  i2c_connector();
  capacitors();
  voltage_regulator();
  power_connector();
  pfet();
  arduino();
  ina260_board();
  i2c_cable();
  power_jst();
}

$fa = 2;
$fs = 0.5;
master_pcb();

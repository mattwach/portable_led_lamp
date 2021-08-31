use <lib/mattwach/util.scad>
use <lib/mattwach/vitamins/electronics/led.scad>
use <lib/mattwach/vitamins/bolts.scad>

module led_dome(led_dome_radius, shell_thickness, base_height, show_leds=true) {
  overlap = 0.01;

  led_dome_diameter = led_dome_radius * 2;

  mount_hole_offset = 9.5;

  low_angle = 20;
  high_angle = 50;
  inset = 1.5;

  module dome() {
    module led_ports() {
      module led_port() {
        wire_hole_x_angle = 60;

        module wire_hole(x_angle) {
          wire_hole_diameter = 6;
          wire_hole_offset = 13;
          tx(led_dome_radius - shell_thickness * 2)
            rx(x_angle) ry(90) hull() {
              cylinder(d=wire_hole_diameter, h=shell_thickness * 2);
              tx(wire_hole_offset) cylinder(d=wire_hole_diameter, h=shell_thickness * 2);
            }
        }

        module mount_hole(hole_offset) {
          mount_hole_diameter = 2.35;
          tz(hole_offset) tx(led_dome_radius - shell_thickness * 2) ry(90)
            cylinder(d=mount_hole_diameter, h=shell_thickness * 2);
        }

        module inset_cutout() {
          inset_cutout_diameter = 22;
          tx(led_dome_radius - inset) ry(90)
            cylinder(d=inset_cutout_diameter, h=inset + overlap);
        }

        module back_hole() {
          back_hole_diameter = 14;
          tx(led_dome_radius - shell_thickness * 2) ry(90)
            cylinder(d=back_hole_diameter, h=shell_thickness * 2);
        }

        union() {
          inset_cutout();
          back_hole();
          wire_hole(wire_hole_x_angle);
          wire_hole(-wire_hole_x_angle);
          wire_hole(180 + wire_hole_x_angle);
          wire_hole(180 - wire_hole_x_angle);
          mount_hole(mount_hole_offset);
          mount_hole(-mount_hole_offset);
        }
      }

      //ry(-low_angle) led_port();
      for (i=[0:3]) {
        rz(i * 90) ry(-low_angle) led_port();
        rz(i * 90 + 45) ry(-high_angle) led_port();
      }
      ry(-90) led_port();
    }

    difference() {
      sphere(r=led_dome_radius);
      sphere(r=led_dome_radius - shell_thickness);
      translate([
          -led_dome_radius - overlap,
          -led_dome_radius - overlap,
          -led_dome_radius - overlap]) cube([
            led_dome_diameter + overlap * 2,
            led_dome_diameter + overlap * 2,
            led_dome_radius + overlap]);
      led_ports();
    }
  }

  module base() {
    difference() {
      cylinder(r=led_dome_radius, h=base_height + overlap);
      tz(-overlap) cylinder(
          r=led_dome_radius - shell_thickness,
          h=base_height + overlap * 3);
    }
  }

  module leds() {
    module led() {
      tx(led_dome_radius - inset) rx(90) ry(90) {
        hex_led_3w();
        tz(1.3) {
          ty(mount_hole_offset) bolt_M2_5(6);
          ty(-mount_hole_offset) bolt_M2_5(6);
        }
      }
    }
    //ry(-low_angle) tx(led_dome_radius) led();
    for (i=[0:3]) {
      rz(i * 90) ry(-low_angle) led();
      rz(i * 90 + 45) ry(-high_angle) led();
    }
    ry(-90) led();
  }

  color("#eee", 0.7) union() {
    tz(base_height) dome();
    base();
  }
  if (show_leds) {
    tz(base_height) leds();
  }
}

/*
$fa = 2;
$fs = 0.5;
led_dome(true);
*/

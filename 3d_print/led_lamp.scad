use <lib/mattwach/util.scad>
use <lib/mattwach/shapes.scad>
use <lib/mattwach/vitamins/bolts.scad>
use <lib/mattwach/vitamins/electronics/barrel_connector.scad>
use <lib/mattwach/vitamins/electronics/picobuck.scad>

use <master_pcb.scad>
use <input_pcb.scad>
use <led_dome.scad>
use <dimmer_knob.scad>

module led_lamp() {
  overlap = 0.01;
  mesh_gap = 0.15;

  base_thickness = 5;

  bolt_hole_size = 1.85;

  barrel_connector_zoffset = 15;

  led_dome_radius = 55;
  led_dome_shell_thickness = 3;
  led_dome_inner_radius = led_dome_radius - led_dome_shell_thickness;

  input_pcb_length = 70;
  input_pcb_width = 30;
  input_pcb_thickness = 1.6;
  input_pcb_xoffset = -input_pcb_length / 2;
  input_pcb_yoffset = -45;
  input_pcb_zoffset = 10;

  input_holder_zpad = 7;
  input_holder_fillet = 10;
  input_holder_inner_fillet = input_holder_fillet - led_dome_shell_thickness;
  input_holder_xsize = led_dome_radius * 2;
  input_holder_zsize = input_pcb_width + input_pcb_zoffset + input_holder_zpad;
  input_holder_pcb_offset = 17;

  input_holder_ysize = -input_pcb_yoffset + input_holder_pcb_offset;

  master_pcb_length = 70;
  master_pcb_width = 50;
  master_pcb_xoffset = -40;
  master_pcb_yoffset = -39;
  master_pcb_zoffset = base_thickness + 10;

  ring_retainer_hole_diameter = 2.35;
  ring_retainer_hole_zoffset = 3.5;

  picobuck_mount_plate_offset = 50;
  pcb_mount_offset = 2.1;
  pcb_mount_hole_diameter = 1.85;

  module base_bolt_hole() {
    base_hole_inner_diameter = 2.35;
    base_hole_outer_diameter = 2.65;
    base_hole_inner_depth = 10;

    union() {
      tz(-overlap) cylinder(d=base_hole_outer_diameter, h=led_dome_shell_thickness + overlap * 2);
      tz(-base_hole_inner_depth) cylinder(d=base_hole_inner_diameter, h=base_hole_inner_depth);
    }
  }

  module base_to_input_holder_bolt_holes() {
    side_offset = 15;

    module left_side_hole() {
     translate([
         -led_dome_inner_radius,
         0,
         base_thickness / 2 
     ]) ry(-90) base_bolt_hole();
    }

    ty(-side_offset) left_side_hole();

    module right_side_hole() {
     translate([
         led_dome_inner_radius,
         0,
         base_thickness / 2 
     ]) ry(90) base_bolt_hole();
    }

    ty(-side_offset) right_side_hole();

    module front_hole() {
     translate([
         0,
         -input_holder_ysize + led_dome_shell_thickness,
         base_thickness / 2 
     ]) rx(90) base_bolt_hole();
    }

    tx(led_dome_radius - side_offset) front_hole();
    front_hole();
    tx(-led_dome_radius + side_offset) front_hole();
  }

  module base_to_led_dome_bolt_holes() {
    bolt_count = 4;
    bolt_spread_degrees = 160;
    module hole() {
      translate([
        0,
        led_dome_inner_radius,
        base_thickness / 2
      ]) rx(-90) base_bolt_hole();
    }

    rz(-80) hole();
    rz(-35) hole();
    rz(35) hole();
    rz(80) hole();
  }

  module barrel_connector_mount(add_hole, size_delta) {
    mount_height = 23 + size_delta;

    module barrel_connector_hole() {
      hole_diameter = 8.2;
      seat_diameter = 12;
      translate([
          0,
          led_dome_inner_radius,
          barrel_connector_zoffset
      ]) rx(-90) union() {
        tz(-overlap) cylinder(d=hole_diameter, h=led_dome_shell_thickness + overlap * 2);
        tz(-2) cylinder(d=seat_diameter, h=2);
        tz(led_dome_shell_thickness - 1) cylinder(d=seat_diameter, h=2);
      }
    }

    intersection() {
      difference() {
        cylinder(r=led_dome_radius + overlap, h=mount_height);
        tz(-overlap) cylinder(r=led_dome_inner_radius - mesh_gap - overlap, h=mount_height + overlap * 2);
        if (add_hole) {
          barrel_connector_hole();      
        }
      }
      rx(-90) cylinder(r=mount_height + overlap, h=led_dome_radius + overlap * 2);
    }
  }


  module lamp_base() {
    module front_corner() {
      ty(-input_holder_ysize + led_dome_shell_thickness + input_holder_inner_fillet)
        cylinder(r=input_holder_inner_fillet, h=base_thickness);
    }

    module master_pcb_mounts() {
      mount_height = master_pcb_zoffset - base_thickness;
      module mount_peg() {
        mount_peg_diameter = 5;
        module peg() {
          rounded_cylinder(d=mount_peg_diameter, h=mount_height + overlap, chamfer=-2);
        }

        module hole() {
          tz(-overlap) cylinder(d=pcb_mount_hole_diameter, h=mount_height + overlap * 3);
        }

        translate([
            master_pcb_xoffset,
            master_pcb_yoffset,
            base_thickness - overlap
        ]) difference() {
          peg();
          hole();
        }
      }

      txy(pcb_mount_offset, pcb_mount_offset) mount_peg();
      txy(master_pcb_length - pcb_mount_offset, pcb_mount_offset) mount_peg();
      txy(master_pcb_length - pcb_mount_offset, master_pcb_width - pcb_mount_offset) mount_peg();
      txy(pcb_mount_offset, master_pcb_width - pcb_mount_offset) mount_peg();
    }

    module main() {
      main_radius = led_dome_inner_radius - mesh_gap;
      
      module tripod_nut() {
        tripod_nut_flat_side_width = 11;
        tripod_nut_height = 5.6;
        tripod_nut_hole_size = 6.7;
        union() {
          tz(1.5) solid_nut(flat_side_width=tripod_nut_flat_side_width, height=tripod_nut_height + overlap);
          tz(-overlap) cylinder(d=tripod_nut_hole_size, h=2);
        }
      }

      difference() {
        hull() {
          cylinder(r=main_radius, h=base_thickness);
          tx(main_radius - input_holder_inner_fillet) front_corner();
          tx(-main_radius + input_holder_inner_fillet) front_corner();
        }
        base_to_input_holder_bolt_holes();
        base_to_led_dome_bolt_holes();
        tripod_nut();
      }
    }

    color("#bbb") union() {
      main();
      master_pcb_mounts();
      barrel_connector_mount(true, 0);
    }
  }

  module input_holder_hull(xsize, ysize, zsize, fillet) {
    module front_top_corner() {
      tz(zsize - fillet) ty(-ysize + fillet) sphere(r=fillet);
    }

    module back_top_corner() {
      tz(zsize - fillet) rx(90) cylinder(r=fillet);
    }

    module front_bottom_corner() {
      ty(-ysize + fillet) cylinder(r=fillet);
    }

    hull() {
      txy(-xsize / 2, -1) cube([xsize, 1, 1]);
      tx(-xsize / 2 + fillet) {
        back_top_corner();
        front_top_corner();
        front_bottom_corner();
      }
      tx(xsize / 2 - fillet) {
        back_top_corner();
        front_top_corner();
        front_bottom_corner();
      }
    }
  }

  module input_holder() {
    input_holder_xpad = 10;

    input_holder_inside_y = -input_holder_ysize + led_dome_shell_thickness + 5;

    module pot_hole() {
      pot_hole_diameter = 10;
      pot_hole_pcb_xoffset = 12;
      pot_hole_pcb_zoffset = 15.4;
      translate([
          input_pcb_xoffset + pot_hole_pcb_xoffset,
          input_holder_inside_y,
          input_pcb_zoffset + pot_hole_pcb_zoffset]) rx(90) cylinder(d=pot_hole_diameter, h=led_dome_shell_thickness + 6);
    }

    module button_hole() {
      button_hole_diameter = 13.5;
      button_hole_pcb_xoffset = 58;
      button_hole_pcb_zoffset = 6;
      translate([
          input_pcb_xoffset + button_hole_pcb_xoffset,
          input_holder_inside_y,
          input_pcb_zoffset + button_hole_pcb_zoffset]) rx(90) cylinder(d=button_hole_diameter, h=led_dome_shell_thickness + 6);
    }

    module led_hole() {
      led_hole_xsize = 30;
      led_hole_zsize = 30;
      led_hole_pcb_xoffset = 20.1;
      led_hole_pcb_zoffset = -1;

      module back_port() {
        translate([
            input_pcb_xoffset + led_hole_pcb_xoffset,
            -input_holder_ysize + 0.5,
            input_pcb_zoffset + led_hole_pcb_zoffset])
              cube([led_hole_xsize, led_dome_shell_thickness + 5, led_hole_zsize]);
      }

      module front_port() {
        front_port_xsize = 27; 
        front_port_zsize = 16; 
        front_port_zoffset = 8.5;
        translate([
            input_pcb_xoffset + led_hole_pcb_xoffset + (led_hole_xsize / 2 - front_port_xsize / 2),
            -input_holder_ysize,
            input_pcb_zoffset + led_hole_pcb_zoffset + front_port_zoffset])
              cube([front_port_xsize, led_dome_shell_thickness + 5, front_port_zsize]);
      }

      union() {
        back_port();
        front_port();
      }
    }

    module pcb_mounts() {
      module pcb_mount_and_hole() {
        pcb_mount_height = input_holder_pcb_offset -
          input_pcb_thickness - led_dome_shell_thickness;
        module pcb_mount() {
          pcb_mount_diameter = 5;
          rounded_cylinder(d=pcb_mount_diameter, h=pcb_mount_height + overlap, chamfer2=-1.5);
        }
        module pcb_mount_hole() {
          tz(-overlap) cylinder(d=pcb_mount_hole_diameter, h=pcb_mount_height + overlap * 3);
        }

        translate([
            input_pcb_xoffset,
            input_pcb_yoffset - input_pcb_thickness,
            input_pcb_zoffset]) rx(90)
          difference() {
            pcb_mount();
            pcb_mount_hole();
          }
      }

      tz(pcb_mount_offset) tx(pcb_mount_offset) pcb_mount_and_hole();
      tz(input_pcb_width - pcb_mount_offset) tx(pcb_mount_offset) pcb_mount_and_hole();
      tz(input_pcb_width - pcb_mount_offset) tx(input_pcb_length - pcb_mount_offset) pcb_mount_and_hole();
      tz(pcb_mount_offset) tx(input_pcb_length - pcb_mount_offset) pcb_mount_and_hole();
    }

    color("#A88", 0.7) union() {
      difference() {
        input_holder_hull(
            xsize=input_holder_xsize,
            ysize=input_holder_ysize,
            zsize=input_holder_zsize,
            fillet=input_holder_fillet
        );
        ty(overlap) tz(-overlap) input_holder_hull(
            xsize=input_holder_xsize - led_dome_shell_thickness * 2,
            ysize=input_holder_ysize -led_dome_shell_thickness + overlap,
            zsize=input_holder_zsize - led_dome_shell_thickness + overlap,
            fillet=input_holder_inner_fillet
        );
        tz(input_holder_zsize - input_holder_fillet) cylinder(r=led_dome_inner_radius, h=input_holder_fillet + overlap);
        led_hole();
        pot_hole();
        button_hole();
        tz(15.2) button_hole();
        base_to_input_holder_bolt_holes();
      }
      pcb_mounts();
    }
  }

  module placed_master_pcb() {
    translate([
        master_pcb_xoffset,
        master_pcb_yoffset,
        master_pcb_zoffset,
    ]) master_pcb();
  }

  module placed_input_pcb() {
    translate([
        input_pcb_xoffset,
        input_pcb_yoffset,
        input_pcb_zoffset]) rx(90) input_pcb();
  }

  module placed_led_dome(show_leds, barrel_connector_cutout) {
    module input_cutout() {
      ty(mesh_gap) input_holder_hull(
          xsize=input_holder_xsize + mesh_gap,
          ysize=input_holder_ysize + mesh_gap,
          zsize=input_holder_zsize + mesh_gap,
          fillet=input_holder_fillet
      );
    }

    module picobuck_mount_plate_hole() {
      tz(picobuck_mount_plate_offset + ring_retainer_hole_zoffset)
        tx(led_dome_inner_radius - 1) ry(90)
        cylinder(d=ring_retainer_hole_diameter, h=led_dome_shell_thickness + 2);
    }

    difference() {
      led_dome(
        led_dome_radius=led_dome_radius,
        base_height=55,
        shell_thickness = led_dome_shell_thickness,
        show_leds=show_leds);
      if (barrel_connector_cutout) {
        tz(-overlap) barrel_connector_mount(false, mesh_gap);
      }
      tz(-overlap) input_cutout();
      base_to_led_dome_bolt_holes();
      for (i=[0:2]) {
        rz(120 * i + 3) picobuck_mount_plate_hole();
      }
    }
  }

  module picobuck_mount_plate(add_picobucks) {
    picobuck_mount_thickness = 2;
    picobuck_mount_radius = led_dome_inner_radius - mesh_gap;
    outer_ring_thickness = 4;
    picobuck_pcb_yoffset = 9;

    bolt_ring_thickness = 5;
    // The calculations below are likely flawed
    inner_bolt_ring_radius = picobuck_pcb_yoffset + 12.9; 
    outer_bolt_ring_radius = picobuck_pcb_yoffset + 34; 

    module ring(radius, thickness, add_bolt_holes=false, bolt_hole_angle=0) {
      module bolt_hole() {
        bolt_hole_diamter = 2.35;
        tx(radius - thickness / 2) tz(-overlap) cylinder(
            d=bolt_hole_diamter, h=picobuck_mount_thickness + overlap * 2);
      }

      difference() {
        cylinder(r=radius, h=picobuck_mount_thickness);
        tz(-overlap) cylinder(r=radius - thickness, h=picobuck_mount_thickness + overlap * 2);
        if (add_bolt_holes) {
          for (i = [0:2]) {
            rz(i * 120 + bolt_hole_angle) bolt_hole();
          }
        }
      }
    }

    module placed_picobuck() {
      picobuck_pcb_width = 26;
      translate([
          -picobuck_pcb_width / 2,
          picobuck_pcb_yoffset,
          picobuck_mount_thickness
      ]) picobuck();
    }

    module ring_retainer() {
      ring_retainer_offset = inner_bolt_ring_radius - bolt_ring_thickness / 2;
      ring_retainer_mount_height = 6;
      rotate_extrude(angle=6) polygon([
          [ring_retainer_offset, 0],
          [picobuck_mount_radius, 0],
          [picobuck_mount_radius, ring_retainer_mount_height],
          [picobuck_mount_radius - outer_ring_thickness, ring_retainer_mount_height],
          [picobuck_mount_radius - outer_ring_thickness, picobuck_mount_thickness],
          [ring_retainer_offset, picobuck_mount_thickness]
      ]);
    }

    module ring_retainer_hole() {
      tz(ring_retainer_hole_zoffset) tx(picobuck_mount_radius - outer_ring_thickness - 1) ry(90)
        cylinder(d=ring_retainer_hole_diameter, h=outer_ring_thickness + 2);
    }

    difference() {
      union() {
        ring(picobuck_mount_radius, outer_ring_thickness);
        ring(outer_bolt_ring_radius, bolt_ring_thickness, add_bolt_holes=true, bolt_hole_angle=-7.7);
        ring(inner_bolt_ring_radius, bolt_ring_thickness, add_bolt_holes=true, bolt_hole_angle=37.3);
        for (i=[0:2]) {
          rz(120 * i) ring_retainer();
        }
      }
      for (i=[0:2]) {
        rz(120 * i + 3) ring_retainer_hole();
      }
    }

    if (add_picobucks) {
      for (i=[0:2]) {
        rz(120 * i) placed_picobuck();
      }
    }
  }

  module placed_picobuck_mount_plate(add_picobucks) {
    tz(picobuck_mount_plate_offset) picobuck_mount_plate(add_picobucks);
  }

  module placed_barrel_connector() {
    translate([
        0,
        led_dome_radius,
        barrel_connector_zoffset
     ]) rx(-90) barrel_mount_female_5_5(hex_nut_gap=led_dome_shell_thickness + 1);
  }

  module placed_dimmer_knob() {
    translate([
      input_pcb_xoffset + 12,
      input_pcb_yoffset - input_holder_pcb_offset - 1,
      input_pcb_zoffset + 15.6,
    ]) rx(90) dimmer_knob();
  }

  placed_master_pcb();
  placed_input_pcb();
  placed_picobuck_mount_plate(add_picobucks=true);
  placed_barrel_connector();
  lamp_base();
  placed_led_dome(show_leds=true, barrel_connector_cutout=true);
  input_holder();
  placed_dimmer_knob();
}

$fa = 2;
$fs = 0.5;
led_lamp();

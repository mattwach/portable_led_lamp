use <../../util.scad>

module picobuck() {
  overlap = 0.01;
  
  led1_xoffset = 2.7;
  led2_xoffset = 11.7;
  led3_xoffset = 20;

  led_y_offset = 28.3;

  input_power_xoffset = 19.9;
  input_power_yoffset = 3.1;

  interface_hole_bottom_offset = 3.4;
  interface_hole_spacing = 2.4;
  interface_pair1_xoffset = 2.2;
  interface_pair2_xoffset = 10.1;

  pcb_thickness = 1.6;
  power_hole_spacing = 3.6;
  tin_thickness = 0.1;

  electronic_group_spacing = 8.6;

  module pcb() {
    pcb_xsize = 26;
    pcb_ysize = 31.2;

    module screw_mount() {
      screw_mount_fillet = 1.25;
      screw_mount_xsize = 4.8;
      screw_mount_ysize = 5.5;
      
      module corner() {
        cylinder(r=screw_mount_fillet, h=pcb_thickness);
      }

      module mount_hole() {
        hole_diameter = 3.2;
        translate([
            screw_mount_xsize / 2,
            screw_mount_ysize / 2,
            -overlap
        ]) cylinder(d=hole_diameter, h=pcb_thickness + overlap * 2);
      }

      difference() {
        hull() {
          cube([1, screw_mount_ysize, pcb_thickness]);
          txy(screw_mount_xsize - screw_mount_fillet, screw_mount_fillet) corner();
          txy(screw_mount_xsize - screw_mount_fillet,
              screw_mount_ysize - screw_mount_fillet) corner();
        }
        mount_hole();
      }
    }

    color("#a00") union() {
      cube([pcb_xsize, pcb_ysize, pcb_thickness]);
      tx(pcb_xsize - overlap) screw_mount();
      txy(overlap, pcb_ysize) rz(180) screw_mount();
    }
  }

  module interface_holes_tin() {
    module tin_pair() {
      module tin() {
        tin_size = 1.5;
        translate([
            -tin_size / 2,
            interface_hole_bottom_offset - tin_size / 2,
            -tin_thickness
        ]) cube([tin_size, tin_size, pcb_thickness + tin_thickness * 2]);
      }

      tin();
      tx(interface_hole_spacing) tin();
    }

    color("#ddd") {
      tx(interface_pair1_xoffset) tin_pair();
      tx(interface_pair2_xoffset) tin_pair();
    }
  }

  module interface_holes() {
    module hole_pair() {
      module hole() {
        interface_hole_diameter = 0.8;
        translate([
            0,
            interface_hole_bottom_offset,
            -tin_thickness - overlap
        ]) cylinder(d=interface_hole_diameter, h=pcb_thickness + tin_thickness * 2 + overlap * 2);
      }

      hole();
      tx(interface_hole_spacing) hole();
    }

    tx(interface_pair1_xoffset) hole_pair();
    tx(interface_pair2_xoffset) hole_pair();
  }

  module power_holes_tin() {
    module tin_pair() {
      square_tin_size = 2.1;
      circle_tin_diameter = 2.1;

      module square_tin() {
        cube([square_tin_size, square_tin_size, pcb_thickness + tin_thickness * 2]);
      }

      module circle_tin() {
        cylinder(d=circle_tin_diameter, h=pcb_thickness + tin_thickness * 2);
      }

      color("#ddd") tz(-tin_thickness) {
        txy(-square_tin_size / 2, -square_tin_size / 2) square_tin();
        tx(power_hole_spacing) circle_tin();
      }

    }

    module led_tin_pair() {
      txy(power_hole_spacing, led_y_offset) rz(180) tin_pair();
    }

    txy(input_power_xoffset, input_power_yoffset) tin_pair();
    tx(led1_xoffset) led_tin_pair();
    tx(led2_xoffset) led_tin_pair();
    tx(led3_xoffset) led_tin_pair();
  }

  module power_holes() {
    module hole_pair() {
      module power_hole() {
        hole_diameter = 1.4;
        tz(-tin_thickness - overlap) cylinder(
            d=hole_diameter, h=pcb_thickness + tin_thickness * 2 + overlap * 2);
      }

      power_hole();
      tx(power_hole_spacing) power_hole();
    }

    module led_hole_pair() {
      txy(power_hole_spacing, led_y_offset) rz(180) hole_pair();
    }

    txy(input_power_xoffset, input_power_yoffset) hole_pair();
    tx(led1_xoffset) led_hole_pair();
    tx(led2_xoffset) led_hole_pair();
    tx(led3_xoffset) led_hole_pair();
  }

  module pcb_with_tin() {
    pcb();
    interface_holes_tin();
    power_holes_tin();
  }

  module electronics_group() {
    module inductor() {
      inductor_diameter = 6;
      induction_height = 4.1;
      inductor_xoffset = 1.2;
      inductor_yoffset = 17.3;
      color("#222") txy(
          inductor_xoffset + inductor_diameter / 2,
          inductor_yoffset + inductor_diameter / 2) cylinder(
            d=inductor_diameter, h=induction_height);
    }

    module capacitor() {
      capacitor_xsize = 2.6;
      capacitor_ysize = 4.1;
      capacitor_height = 2.3;
      capacitor_xoffset = 5.6;
      capacitor_yoffset = 10.9;
      color("#222") txy(
          capacitor_xoffset,
          capacitor_yoffset) cube([capacitor_xsize, capacitor_ysize, capacitor_height]);
    }

    module controller() {
      controller_xsize = 2.8;
      controller_ysize = 1.6;
      controller_height = 1.3;
      controller_xoffset = 2.4;
      controller_yoffset = 12.5;
      color("#222") txy(
          controller_xoffset,
          controller_yoffset) cube([controller_xsize, controller_ysize, controller_height]);
    }

    tz(pcb_thickness) {
      inductor();
      capacitor();
      controller();
    }
  }

  difference() {
    pcb_with_tin();
    interface_holes();
    power_holes();
  }
  for (i=[0:2]) {
    tx(i * electronic_group_spacing) electronics_group();
  }
}

/*
$fa=2;
$fs=0.5;
picobuck();
*/


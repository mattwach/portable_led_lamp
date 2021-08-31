use <../../util.scad>

overlap = 0.01;

module potentiometer_screw_7mm() {
  base_width = 7;
  base_height = 7;
  base_thickness = 2.9; 
  pin_length = 3;
  pin_spacing = 5.1;

  module base() {
    color("#222") cube([base_width, base_thickness, base_height]);
  }

  module dial() {
    diameter = 4.4;
    thickness = 1.3;
    cross_depth = 1;

    module cross() {
      width = 0.9;
      length = 2.8;
      module segment() {
        tz(cross_depth / 2) cube([width, length, cross_depth], center=true); 
      }
      union() {
        segment();
        rz(90) segment();
      }
    }

    difference() {
      color("#f80") union() {
        cylinder(d=diameter, h=thickness);
        ty(-diameter / 2) cube([diameter * 0.6, diameter, thickness]);
      }
      tz(thickness - cross_depth + overlap) cross();
    }
  }

  module pin() {
    diameter = 0.9;
    color("silver") cylinder(d=diameter, h=pin_length);
  }

  base();
  tz(base_height / 2) tx(base_width / 2) ry(-60) rx(90) dial();
  translate([base_width / 2, 0, -pin_length]) pin();
  translate([base_width / 2 - pin_spacing / 2, base_thickness, -pin_length]) pin();
  translate([base_width / 2 + pin_spacing / 2, base_thickness, -pin_length]) pin();
}

module potentiometer_plate_mount() {
  board_length = 16.2;
  board_thickness = 1;
  base_diameter = 12.8;
  base_height = 4.8;
  mid_section_height = 3;
  pin_spacing = 3.7;
  threads_diameter = 6.8;
  threads_height = 7;

  module base() {
    module side_notch() {
      side_notch_width = 1.5;
      side_notch_height = 2;
      side_notch_depth = 1.3;
      translate([
          -side_notch_width,
          base_diameter / 2 - side_notch_depth,
          -overlap]) cube([
            side_notch_width,
            side_notch_depth + overlap,
            side_notch_height + overlap]);
    }

    color("#ddd") difference() {
      cylinder(d=base_diameter, h=base_height);
      side_notch();
    }
  }

  module board() {

    color("#622") tz(base_height) union() {
      cylinder(d=base_diameter, h=board_thickness);
      ty(-base_diameter / 2) cube([
          board_length - base_diameter / 2,
          base_diameter,
          board_thickness
      ]);
    }
  }

  module pin() {
    pin_thickness = 0.5;
    pin_top_width = 1.4;
    pin_top_length = 4.8;
    pin_top_outlay = 1;
    pin_front_narrow_height = 1.3;
    pin_front_wide_height = 2.7;
    pin_front_wide_width = 2.1;
    pin_hole_pad = 0.5;

    color("#ddd") union() {
      translate([
        board_length - base_diameter / 2 - pin_top_length + pin_top_outlay,
        -pin_top_width / 2,
        base_height + board_thickness
      ]) cube([pin_top_length, pin_top_width, pin_thickness]);
      translate([
        board_length - base_diameter / 2 + pin_top_outlay - pin_thickness,
        -pin_top_width / 2,
        base_height + board_thickness - pin_front_narrow_height
      ]) cube([pin_thickness, pin_top_width, pin_front_narrow_height + overlap]);
      difference() {
        translate([
          board_length - base_diameter / 2 + pin_top_outlay - pin_thickness,
          -pin_front_wide_width / 2,
          base_height + board_thickness - pin_front_narrow_height - pin_front_wide_height
        ]) cube([
          pin_thickness,
          pin_front_wide_width,
          pin_front_wide_height + overlap]);
        translate([
          board_length - base_diameter / 2 + pin_top_outlay - pin_thickness - overlap,
          -pin_front_wide_width / 2 + pin_hole_pad,
          base_height + board_thickness - pin_front_narrow_height - pin_front_wide_height + pin_hole_pad
        ]) cube([
          pin_thickness + overlap * 2,
          pin_front_wide_width - pin_hole_pad * 2,
          pin_front_wide_height - pin_hole_pad * 2]);
      }
    }
  }

  module mid_section() {
    mid_section_diameter = 8.7;
    module tube() {
      cylinder(d=mid_section_diameter, h=mid_section_height);
    }

    module brace() {
      post_width = 2;
      post_height = 3.4;
      brace_width = 3.9;
      brace_height = 2.1;
      brace_length1 = 10.4 - mid_section_diameter / 2;
      brace_length2 = 11.6 - mid_section_diameter / 2;
      tz(mid_section_height - brace_height) linear_extrude(brace_height) polygon([
          [0, -brace_width / 2],
          [brace_length1, -brace_width / 2],
          [brace_length2, -post_width / 2],
          [brace_length2, post_width / 2],
          [brace_length1, brace_width / 2],
          [0, brace_width / 2],
      ]);
      translate([
          brace_length2 + post_width / 4,
          0,
          mid_section_height - brace_height
      ])
      cylinder(d=post_width, h=post_height);
    }

    tz(base_height + board_thickness) color("#ddd") union() {
      tube();
      brace();
    }
  }

  module threads() {
    color("#bbb") tz(base_height + board_thickness + mid_section_height)
      cylinder(d=threads_diameter, h=threads_height);
  }

  module knob() {
    knob_narrow_height = 2.3;
    knob_top_diameter = 5.9;
    knob_top_height = 5.7;
    module knob_base() {
      knob_narrow_diameter = 4.8;
      knob_chamfer = (threads_diameter - knob_narrow_diameter) / 2;
      union() {
        cylinder(d1=threads_diameter, d2=knob_narrow_diameter, h=knob_chamfer + overlap);
        tz(knob_chamfer) cylinder(
            d=knob_narrow_diameter, h=knob_narrow_height - knob_chamfer + overlap);
      }
    }

    module knob_top() {
      ridge_count = 20;

      module ridge() {
        ridge_diameter = 0.5;
        tx(knob_top_diameter / 2) cylinder(d=ridge_diameter, h=knob_top_height + overlap * 2);
      }

      tz(knob_narrow_height) difference() {
        cylinder(d=knob_top_diameter, h=knob_top_height);
        for (i=[0:ridge_count-1]) {
          rz(i * 360 / ridge_count) ridge();
        }
      }
    }

    module knob_slot() {
      slot_width = 0.9;
      slot_height = 6.5;
      translate([
          -slot_width / 2,
          -knob_top_diameter / 2 - overlap,
          knob_narrow_height + knob_top_height - slot_height
      ]) cube([slot_width, knob_top_diameter + overlap * 2, slot_height + overlap]);
    }

    color("#ddd") tz(base_height + board_thickness + mid_section_height + threads_height)
      difference() {
        union() {
          knob_base();
          knob_top();
        }
        knob_slot();
      }
  }

  base();
  board();
  mid_section();
  threads();
  knob();
  pin();
  ty(pin_spacing) pin();
  ty(-pin_spacing) pin();
}

/*
$fa=2;
$fs=0.5;
potentiometer_screw_7mm();
potentiometer_plate_mount();
*/


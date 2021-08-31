use <../../util.scad>

module jst_female() {
  length = 19.1;
  front_length = 9.5;
  front_width = 6.8;
  height = 4;
  back_length = length - front_length;
 
  overlap = 0.01;

  module front() {
    pad = 0.6;
    module key() {
      key_width = 1.2;
      key_height = 0.3;
      translate([
          0,
          (front_width - key_width) / 2,
          height - pad - key_height]) cube([
          front_length,
          key_width,
          key_height + overlap]);
    }

    tx(back_length) union() {
      difference() {
        cube([front_length, front_width, height]);
        translate([
            -overlap,
            pad,
            pad
        ]) cube([
          front_length + overlap * 2, front_width - pad * 2, height - pad * 2]);
      }
      key();
    }
  }

  module back() {
    hole_top_pad = 0.7;
    hole_ipad = 0.4;
    hole_width = 2.2;
    back_width = 5.9;
    y_offset = (front_width - back_width) / 2;
    module hole() {

      module notch() {
        notch_width = 1.3;
        notch_height = 0.4;
        tz(-notch_height) ty((hole_width - notch_width) / 2)
          cube([back_length + overlap * 4, notch_width, notch_height + overlap * 2]);
      }

      hole_height = 2.2;
      translate([
          -overlap,
          y_offset + (back_width - hole_width) / 2,
          height - hole_height - hole_top_pad
      ]) union() {
        cube([back_length + overlap * 4, hole_width, hole_height]);
        notch();
      }
    }

    difference() {
      ty(y_offset)
        cube([back_length + overlap, back_width, height]);
      ty(-hole_width / 2 - hole_ipad / 2) hole();
      ty(hole_width / 2 + hole_ipad / 2) hole();
    }
  }

  module label(label_text) {
    translate([
        1,
        2,
        height - overlap
    ]) linear_extrude(0.15) rz(-90) text(label_text, size=2, halign="center");
  }

  color("red") translate([0, -2.1, -2]) union() {
    back();
    front();
    label("1");
    ty(2.54) label("2");
  }
}

module jst_male() {
  length = 13.1;
  back_length = 0.5;
  middle_length = 4.3 - back_length;
  width = 5.3;
  height = 3.2;

  overlap = 0.01;

  module back() {
    back_width = 6.3;
    ty(-(back_width - width) / 2) cube([back_length, back_width, height]);
  }

  module front() {
    chamfer = 0.9;
    nub_width = (width - chamfer * 3) / 2;
    translate([back_length + middle_length - overlap, width, height]) 
      rz(-90) rx(-90) linear_extrude(length - back_length - middle_length + overlap) polygon([
          [0,0],
          [width, 0],
          [width, height - chamfer],
          [width - chamfer, height],
          [width - chamfer - nub_width, height],
          [width - chamfer - nub_width, height - chamfer],
          [width - chamfer * 2 - nub_width, height - chamfer],
          [width - chamfer * 2 - nub_width, height],
          [width - chamfer * 2 - nub_width * 2, height],
          [0, height - chamfer],
      ]);
  }

  module middle() {
    tx(back_length - overlap) cube([middle_length + overlap * 2, width, height]);
  }

  module front_holes() {
    front_hole_span = 2.54;
    module hole() {
      front_hole_width = 1.5;
      front_hole_height = 1;
      front_hole_top_pad = 0.75;
      translate([
          back_length + middle_length,
          width / 2 - front_hole_width / 2,
          height - front_hole_top_pad - front_hole_height
      ]) cube([length - middle_length, front_hole_width, front_hole_height]);
    }
    ty(front_hole_span / 2) hole();
    ty(-front_hole_span / 2) hole();
  }

  module key_holes() {
    key_hole_span = 2.54;

    module key_hole() {
      key_hole_width = 1.4;
      key_hole_height = 1.8;
      key_hole_length = 2;
      key_hole_end_pad = 2;
      translate([
          length - key_hole_length - key_hole_end_pad,
          (width - key_hole_width) / 2,
          -overlap]) cube([
            key_hole_length,
            key_hole_width,
            key_hole_height + overlap]);
    }

    ty(key_hole_span / 2) key_hole();
    ty(-key_hole_span / 2) key_hole();
  }

  module back_holes() {
    back_hole_span = 2.54;

    module back_hole() {
      back_hole_width = 2.0;
      back_hole_height = 2.0;
      back_hole_top_pad = 0.4;
      
      module notch() {
        notch_width = 1.6;
        notch_height = 0.4;
        ty((back_hole_width - notch_width) / 2) tz(-notch_height) cube([
            back_length + middle_length + 1,
            notch_width,
            notch_height + overlap]);
      }

      translate([
          -overlap,
          (width - back_hole_width) / 2,
          height - back_hole_height - back_hole_top_pad,
      ]) union() {
        cube([back_length + middle_length + 1, back_hole_width, back_hole_height]);
        notch();
      }

    }

    ty(-back_hole_span / 2) back_hole();
    ty(back_hole_span / 2) back_hole();
  }

  module label(label_text) {
    translate([
        1,
        1.5,
        -0.15 + overlap
    ]) linear_extrude(0.15) rz(90) rx(180) text(label_text, size=2, halign="center");
  }

  color("red") translate([0, -1.4, -1.6]) difference() {
    union() {
      back();
      middle();
      front();
      label("1");
      ty(2.54) label("2");
    }
    front_holes();
    key_holes();
    back_holes();
  }
}


/*
$fa=2.0;
$fs=0.5;
ty(10) jst_female();
jst_male();
*/


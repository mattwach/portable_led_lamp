module small_vreg(pin_length = 4, pin_diameter = 0.5, hover = 2) {
  overlap = 0.1;
  pin_spacing = 2.54;

  module pin() {
    translate([0, 0, -pin_length + hover]) cylinder(d=pin_diameter, h=pin_length + overlap);
  }

  module body() {
    width = 4.8;
    height = 4.5;
    overhang = 0.9;
    color("#333")
      translate([0, 0, hover])
      union() {
      difference() {
        cylinder(d=width, h=height);
        translate([-width - overlap, -(width + overlap) / 2, -overlap]) cube([width + overlap, width + overlap * 2, height + overlap * 2]);
      }
      translate([-overhang, -width / 2, 0]) cube([overhang + overlap, width, height]);
    }
  }

  translate([0, pin_spacing / 2, 0]) body();
  pin();
  translate([0, pin_spacing, 0]) pin();
  translate([0, pin_spacing / 2, 0]) pin();
}

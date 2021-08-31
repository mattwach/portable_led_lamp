
// Parameters:
// d: diameter
// h: height
// fillet: lower fillet.  <0 fillets out, >= fillets in, ==0 no fillet
// chamfer: lower fillet.  <0 fillets out, >= fillets in, ==0 no fillet
module rounded_cylinder(d=1, h=1, fillet=0, chamfer=0, fillet2=0, chamfer2=0) {
  assert(fillet != 0 || chamfer != 0 || fillet2 != 0 || chamfer2 != 0, "fillet or chamfer should be non-zero");
  assert(!(fillet == 0 && chamfer == 0 && fillet2 == 0 && chamfer2 == 0), "filler of chamfer should be zero");
  clearance = abs(fillet) + abs(chamfer);
  clearance2 = abs(fillet2) + abs(chamfer2);
  overlap = 0.01;

  main_zoffset = clearance > 0 ? clearance - overlap : 0;
  main_height = clearance > 0 && clearance2 > 0 ?
    h - clearance - clearance2 + overlap * 2 :
    h - clearance - clearance2 + overlap;

  module inner_fillet(r) {
    union() {
      rotate_extrude() {
        translate([d / 2 - r, r, 0]) circle(r=r);
      }
      cylinder(d=d - r * 2, h=r * 2);
    }
  }

  union() {
    translate([0, 0, main_zoffset]) cylinder(d=d, h=main_height);

    if (fillet > 0) {
      inner_fillet(clearance);
    }

    if (fillet2 > 0) {
      translate([0, 0, h - clearance2 * 2]) inner_fillet(clearance2);
    }

    if (fillet < 0) {
      difference() {
        cylinder(d=d + clearance * 2, h=clearance);
        rotate_extrude() {
          translate([d / 2 + clearance, clearance, 0]) circle(r=clearance);
        }
      }
    }

    if (fillet2 < 0) {
      translate([0, 0, h - clearance2]) difference() {
        cylinder(d=d + clearance2 * 2, h=clearance2);
        translate([0, 0, -clearance2]) rotate_extrude() {
          translate([d / 2 + clearance2, clearance2, 0]) circle(r=clearance2);
        }
      }
    }

    if (chamfer != 0) {
      cylinder(d1=d - chamfer * 2, d2=d, h=clearance); 
    }

    if (chamfer2 != 0) {
      translate([0, 0, h - clearance2])
        cylinder(d1=d, d2=d - chamfer2 * 2, h=clearance2); 
    }
  }
}

// A cube with rounded corners on the xy plane
module rounded_cube_xy(size, fillet) {
  xsize = size[0];
  ysize = size[1];
  zsize = size[2];
  hull() {
    translate([fillet, fillet, 0]) cylinder(r=fillet, h=zsize);
    translate([xsize - fillet, fillet, 0]) cylinder(r=fillet, h=zsize);
    translate([xsize - fillet, ysize - fillet, 0]) cylinder(r=fillet, h=zsize);
    translate([fillet, ysize - fillet, 0]) cylinder(r=fillet, h=zsize);
  }
}

/*
$fn=36;
translate([0, 0, 0]) cylinder(d=5, h=10);
color("red") translate([-8, 0, 0]) rounded_cylinder(d=5, h=10, fillet=1);
color("green") translate([-16, 0, 0]) rounded_cylinder(d=5, h=10, fillet=-1);
color("purple") translate([-24, 0, 0]) rounded_cylinder(d=5, h=10, chamfer=1);
color("orange") translate([-32, 0, 0]) rounded_cylinder(d=5, h=10, chamfer=-1);

translate([0, 8, 0]) cylinder(d=5, h=10);
color("red") translate([-8, 8, 0]) rounded_cylinder(d=5, h=10, fillet2=1);
color("green") translate([-16, 8, 0]) rounded_cylinder(d=5, h=10, fillet2=-1);
color("purple") translate([-24, 8, 0]) rounded_cylinder(d=5, h=10, chamfer2=1);
color("orange") translate([-32, 8, 0]) rounded_cylinder(d=5, h=10, chamfer2=-1);

translate([0, 16, 0]) cylinder(d=5, h=10);
color("red") translate([-8, 16, 0]) rounded_cylinder(d=5, h=10, fillet=1, fillet2=1);
color("green") translate([-16, 16, 0]) rounded_cylinder(d=5, h=10, fillet=-1, fillet2=-2);
color("purple") translate([-24, 16, 0]) rounded_cylinder(d=5, h=10, chamfer=1, chamfer2=2);
color("orange") translate([-32, 16, 0]) rounded_cylinder(d=5, h=10, chamfer=-1, chamfer2=-2);
*/


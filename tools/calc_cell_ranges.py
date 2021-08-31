#!/usr/bin/env python3

import sys


def check_ranges(min_cell_mv, max_cell_mv, print_results):
  cell_count = 1;

  while True:
    if cell_count * min_cell_mv > 30000:
      break
    if cell_count >= 12:
      break

    overlap = (cell_count > 1) and (
        cell_count * min_cell_mv <= ((cell_count - 1) * max_cell_mv))

    if print_results:
      print("Cells: %d  Min: %-6d  Max: %-6d  %s" % (
            cell_count,
            cell_count * min_cell_mv,
            cell_count * max_cell_mv,
            "OVERLAP" if overlap else "OK"))

    if overlap:
      return False

    cell_count += 1

  return True

def main():
  if len(sys.argv) != 2:
    sys.exit("Usage show_cell_ranges <max_cell_mv>")

  max_cell_mv = int(sys.argv[1])

  # binary search
  highest_ok_mv = max_cell_mv
  mv_range = max_cell_mv
  while mv_range > 0:
    test_mv = highest_ok_mv - mv_range
    print("test_mv: %d" % test_mv)
    if check_ranges(test_mv, max_cell_mv, True):
      print("  accepted")
      highest_ok_mv = test_mv
    else:
      print("  rejected")
    mv_range //= 2

  print("--- Final ---")
  final_mv = highest_ok_mv // 10 * 10
  if (highest_ok_mv % 10 != 0):
    final_mv += 10
  check_ranges(final_mv, max_cell_mv, True)
  print("min_cell_mv: %d" % final_mv)



main()

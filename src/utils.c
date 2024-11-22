/**
 * @file utils.c
 * Utility functions etc
 */

#include "util.h"

/**
 * Check if a page's permissions are more or less strict than the permissions
 * requested
 *
 * Return true if the request would be permitted. EG. read request to a R/W page
 * is allowed, but W to an RO page isn't
 */
bool check_permissions(permissions_t permissions_req,
                       permissions_t permissions_page) {
  bool read_check = true;
  bool write_check = true;
  bool execute_check = true;

  // Only set to false if the permissions request read
  if (permissions_req.read == 1) {
    if (permissions_page.read != 1) {
      read_check = false;
    }
  }

  if (permissions_req.write == 1) {
    if (permissions_page.write != 1) {
      write_check = false;
    }
  }

  if (permissions_req.execute == 1) {
    if (permissions_page.execute != 1) {
      execute_check = false;
    }
  }

  return read_check && write_check && execute_check;
}

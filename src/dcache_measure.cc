
/***************************************************************************************
 * File         :
 * Author       :
 * Date         : 10/2024
 * Description  :
 ***************************************************************************************/

#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>

extern "C" {
#include "globals/assert.h"
#include "globals/global_types.h"
#include "globals/global_defs.h"

#include "op.h"
}

#include "dcache_measure.h"

/**************************************************************************************/
/* Structure */

std::unordered_set<Addr> addr_set;

/**************************************************************************************/
/* Interface */

void dcache_measure_init(void) {
  return;
}

Flag dcache_measure_examine(Addr line) {
  if (addr_set.count(line)) {
    return TRUE;
  }

  addr_set.insert(line);
  return FALSE;
}

/***************************************************************************************
 * File         :
 * Author       :
 * Date         : 10/2024
 * Description  :
 ***************************************************************************************/

#include <string>
#include <iostream>
#include <vector>
#include <deque>
#include <unordered_set>
#include <bitset>

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
std::deque<std::bitset<32>> fully_assoc_cache;
int DC_SIZE;
int OFFSET;

/**************************************************************************************/
/* Interface */

void dcache_measure_init(int dcache_size) {
  DC_SIZE = dcache_size;
}

/*
    TODO:
      calculate the offset correctly. It'll be log_2(DC_SIZE) 
        - It's a fully associative set. Offset is just the index in the set
        - We should bitshift line by offset to get the tag to compare to in the set
      Put update_LRU_on_cache_hit in the right place in code in dcache_stage.c
*/
// Reorders the fully associative cache to represent the LRU
void update_LRU_on_cache_hit(Addr line) {
  int offset = 3; // Need to calculate this properly from DC_SIZE
  std::bitset<32> line_entry{line};
  line_entry = line_entry >> (offset+1);
  for (int i = 0; i < fully_assoc_cache.size(); i++)
  {
    if (fully_assoc_cache[i] == line_entry)
    {
      fully_assoc_cache.erase(fully_assoc_cache.begin() + i);
      break;
    }
  }
  fully_assoc_cache.push_back(line_entry);
}

// Returning
// Compulsory: 0, Conflict: 1, Capacity: 2
int dcache_measure_examine(Addr line) {
  if (addr_set.count(line)) {
    // Make sure line gets a bitshift
    for (int i = 0; i < fully_assoc_cache.size(); i++)
    {
      if (fully_assoc_cache[i] == line)
        return 1; // Found in fully assoc cache, Conflict
    }
    return 2; // Not in associative cache, Capacity
  }

  // Make sure line gets a bitshift
  addr_set.insert(line);
  fully_assoc_cache.push_back(line);
  if (fully_assoc_cache.size() > DC_SIZE) {
    fully_assoc_cache.pop_front();
  }
  return 0; // Never seen, Compulsory
}
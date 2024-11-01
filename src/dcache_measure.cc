
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

std::unordered_set<Addr> infinite_cache;
std::deque<std::bitset<32>> fully_assoc_cache;

int FULLY_ASSOC_DCACHE_SIZE;
int OFFSET;

typedef enum Cache_Miss_Type_enum {
  CACHE_MISS_TYPE_COMPULSORY,
  CACHE_MISS_TYPE_CAPACITY,
  CACHE_MISS_TYPE_CONFLICT,
} Cache_Miss_Type;

/**************************************************************************************/
/* Static Methods */

/*
  TODO:
    calculate the offset correctly. It'll be log_2(FULLY_ASSOC_DCACHE_SIZE) 
      - It's a fully associative set. Offset is just the index in the set
      - We should bitshift line by offset to get the tag to compare to in the set
    Put update_LRU_on_cache_hit in the right place in code in dcache_stage.c
*/
// Reorders the fully associative cache to represent the LRU
void update_LRU_on_cache_hit(Addr line) {
  int offset = 3; // Need to calculate this properly from FULLY_ASSOC_DCACHE_SIZE
  std::bitset<32> line_entry{line};
  line_entry = line_entry >> (offset+1);
  for (size_t i = 0; i < fully_assoc_cache.size(); i++)
  {
    if (fully_assoc_cache[i] == line_entry)
    {
      fully_assoc_cache.erase(fully_assoc_cache.begin() + i);
      break;
    }
  }
  fully_assoc_cache.push_back(line_entry);
}

Cache_Miss_Type get_cache_miss_type(Addr line) {
  // Check if the addr has appeared before
  if (!infinite_cache.count(line)) {
    // Insert into a hash map
    infinite_cache.insert(line);

    // Make sure line gets a bitshift
    fully_assoc_cache.push_back(line);
    if (fully_assoc_cache.size() > static_cast<size_t>(FULLY_ASSOC_DCACHE_SIZE))
      fully_assoc_cache.pop_front();

    return CACHE_MISS_TYPE_COMPULSORY;
  }

  // Make sure line gets a bitshift
  for (size_t i = 0; i < fully_assoc_cache.size(); i++) {
    if (fully_assoc_cache[i] == line)
      return CACHE_MISS_TYPE_CONFLICT;
  }

  return CACHE_MISS_TYPE_CAPACITY;
}

/**************************************************************************************/
/* Interface */

void dcache_measure_init(int dcache_size) {
  FULLY_ASSOC_DCACHE_SIZE = dcache_size;
}

void dcache_measure_examine(Op *op, Addr line) {
  Cache_Miss_Type dcache_miss_type = get_cache_miss_type(op->oracle_info.va);
  switch(dcache_miss_type) {
    case CACHE_MISS_TYPE_COMPULSORY:
      STAT_EVENT(op->proc_id, DCACHE_MISS_COMPULSORY);
      break;

    case CACHE_MISS_TYPE_CAPACITY:
      STAT_EVENT(op->proc_id, DCACHE_MISS_CAPACITY);
      break;

    case CACHE_MISS_TYPE_CONFLICT:
      STAT_EVENT(op->proc_id, DCACHE_MISS_CONFLICT);
      break;

    default:
      break;
  }
}
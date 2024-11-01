
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
#include <bits/stdc++.h>

extern "C" {
#include "globals/assert.h"
#include "globals/global_types.h"
#include "globals/global_defs.h"

#include "op.h"
}

#include "dcache_measure.h"

/**************************************************************************************/
/* Structure */

std::unordered_set<std::bitset<32>> infinite_cache;
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
  calculate the offset correctly. It'll be log_2(FULLY_ASSOC_DCACHE_SIZE) 
    - It's a fully associative set. Offset is just the index in the set
    - We should bitshift line by offset to get the tag to compare to in the set
*/

// Reorders the fully associative cache to represent the LRU
void update_LRU_on_cache_hit(Addr line) {
  std::bitset<32> line_entry{line};
  line_entry = line_entry >> OFFSET;
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
  std::bitset<32> line_entry{line};
  line_entry = line_entry >> OFFSET;
  if (!infinite_cache.count(line_entry)) {
    // Insert into a hash map
    infinite_cache.insert(line_entry);

    // Add line_entry not line since we need to account for offset
    fully_assoc_cache.push_back(line_entry);
    if (fully_assoc_cache.size() > static_cast<size_t>(FULLY_ASSOC_DCACHE_SIZE))
      fully_assoc_cache.pop_front();

    return CACHE_MISS_TYPE_COMPULSORY;
  }

  for (size_t i = 0; i < fully_assoc_cache.size(); i++) {
    if (fully_assoc_cache[i] == line_entry)
      return CACHE_MISS_TYPE_CONFLICT;
  }

  return CACHE_MISS_TYPE_CAPACITY;
}

/**************************************************************************************/
/* Interface */

void dcache_measure_init(int dcache_size) {
  FULLY_ASSOC_DCACHE_SIZE = dcache_size;
  OFFSET = log2(FULLY_ASSOC_DCACHE_SIZE);
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
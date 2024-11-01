
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
std::deque<Addr> fully_assoc_cache;

uns FULLY_ASSOC_SIZE;
uns FULLY_ASSOC_SHIFT;

typedef enum Cache_Miss_Type_enum {
  CACHE_MISS_TYPE_COMPULSORY,
  CACHE_MISS_TYPE_CAPACITY,
  CACHE_MISS_TYPE_CONFLICT,
} Cache_Miss_Type;

/**************************************************************************************/
/* Static Methods */

/*
  calculate the offset correctly. It'll be log_2(FULLY_ASSOC_SIZE) 
    - It's a fully associative set. Offset is just the index in the set
    - We should bitshift line by offset to get the tag to compare to in the set
*/

// Reorders the fully associative cache to represent the LRU
static inline void update_LRU_on_cache_hit(Addr addr_line) {
  addr_line = addr_line >> FULLY_ASSOC_SHIFT;
  for (size_t i = 0; i < fully_assoc_cache.size(); i++)
  {
    if (fully_assoc_cache[i] == addr_line)
    {
      fully_assoc_cache.erase(fully_assoc_cache.begin() + i);
      break;
    }
  }
  fully_assoc_cache.push_back(addr_line);
}

static inline Cache_Miss_Type get_cache_miss_type(Addr addr_line) {
  // Check if the addr has appeared before
  if (!infinite_cache.count(addr_line)) {
    // Insert into a hash map
    infinite_cache.insert(addr_line);

    // Add line_entry not line since we need to account for offset
    fully_assoc_cache.push_back(addr_line);
    if (fully_assoc_cache.size() > static_cast<size_t>(FULLY_ASSOC_SIZE))
      fully_assoc_cache.pop_front();

    return CACHE_MISS_TYPE_COMPULSORY;
  }

  for (size_t i = 0; i < fully_assoc_cache.size(); i++) {
    if (fully_assoc_cache[i] == addr_line)
      return CACHE_MISS_TYPE_CONFLICT;
  }

  return CACHE_MISS_TYPE_CAPACITY;
}

static inline void update_cache_miss_stat(Op *op, Addr line_addr) {
  Cache_Miss_Type dcache_miss_type = get_cache_miss_type(line_addr);
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

/**************************************************************************************/
/* External Interface */

void dcache_measure_init(uns cache_size, uns line_size) {
  FULLY_ASSOC_SIZE = cache_size / line_size;
  FULLY_ASSOC_SHIFT = LOG2(FULLY_ASSOC_SIZE);
}

void dcache_measure_examine(Op *op, Addr line_addr, Flag if_miss) {
  if (if_miss)
    update_cache_miss_stat(op, line_addr);
  else
    update_LRU_on_cache_hit(line_addr);
}

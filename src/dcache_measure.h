
/***************************************************************************************
 * File         : dcache_measure.h
 * Author       :
 * Date         : 10/2024
 * Description  : Measure for DCACHE 3C
 ***************************************************************************************/

#ifndef __DCACHE_MEASURE_H__
#define __DCACHE_MEASURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "globals/global_types.h"
#include "op.h"

void dcache_measure_init(uns cache_size, uns line_size);
void dcache_measure_examine(Op *op, Addr line_addr, Flag if_miss);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __DCACHE_MEASURE_H__ */

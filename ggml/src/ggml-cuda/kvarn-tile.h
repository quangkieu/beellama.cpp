#pragma once

#include <cstdint>

// Per-group precision tier for the non-SWA KVarN store ring. Tile 0 is the
// pinned native sink (never quantized); tiles 1..tail_groups rotate through
// the F16 stage; the rest are quantized records. SWA has no sink and uses its
// own ring mapping, so this tier applies to non-SWA groups only.
enum group_zone {
    ZONE_SINK,     // pinned native, never quantized (tile 0)
    ZONE_TAIL,     // rotating F16 ring, pending quantization
    ZONE_RECORD,   // quantized to records
};

// Classify a non-SWA group by its position in the store ring. The sink is
// always tile 0; the tail window is tiles 1..tail_groups; everything beyond
// is already quantized to records. Usable from both host (unit tests) and
// device (store kernels).
#if defined(__CUDACC__)
__host__ __device__ inline group_zone group_zone_for(int group, int tail_groups) {
#else
inline group_zone group_zone_for(int group, int tail_groups) {
#endif
    if (group == 0) return ZONE_SINK;
    if (group <= tail_groups) return ZONE_TAIL;
    return ZONE_RECORD;
}

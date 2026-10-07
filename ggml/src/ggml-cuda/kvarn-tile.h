#pragma once

#include <cstdint>

// Per-group precision tier for the non-SWA KVarN store ring. Tile 0 is the
// pinned native sink (never quantized); tiles 1..tail_groups rotate through
// the F16 stage; the rest are quantized records. SWA has no sink and uses its
// own ring mapping, so this tier applies to non-SWA groups only.
enum class kv_tier : uint8_t {
    SINK,     // pinned native, never quantized (tile 0)
    TAIL,     // rotating F16 ring, pending quantization
    RECORD,   // quantized to records
    // future: SWA, REMOTE, RAM, DISK
};

// Classify a non-SWA group by its position in the store ring. The sink is
// always tile 0; the tail window is tiles 1..tail_groups; everything beyond
// is already quantized to records. Usable from both host (unit tests) and
// device (store kernels).
#if defined(__CUDACC__)
__host__ __device__ inline kv_tier kvarn_group_tier_for(int group, int tail_groups) {
#else
inline kv_tier kvarn_group_tier_for(int group, int tail_groups) {
#endif
    if (group == 0) return kv_tier::SINK;
    if (group <= tail_groups) return kv_tier::TAIL;
    return kv_tier::RECORD;
}

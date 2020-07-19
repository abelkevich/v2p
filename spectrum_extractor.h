#pragma once
#include "cmn_defs.h"

#pragma pack(push,1)
typedef struct FreqRec
{
    uint16_t freq;
    float amp;
} FreqRec;
#pragma pack(pop)

FreqRec* break_into_parts_and_find_lead_freqs(const freq_t freq_window, const ms_t time_to_break_ms, uint32_t *_parts_n,
                                              const float* pcm_buffer, const smpn_t sample_rate, const smpn_t samples_n);
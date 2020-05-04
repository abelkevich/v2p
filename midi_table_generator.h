#pragma once
#include "cmn_defs.h"
#include "freqs_table_generator.h"

#pragma pack(push,1)
struct MidiRec
{
    uint8_t code;
    uint8_t velocity;    
};
#pragma pack(pop)

typedef struct MidiRec MidiRec;

MidiRec* from_freqs_table_to_midi_table(const FreqRec* freqs_table, const uint32_t freqs_table_recs_n,
                                        const float* semitones_table, const uint32_t semitones_n,
                                        uint32_t *_midi_table_recs_n);
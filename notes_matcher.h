#pragma once
#include "cmn_defs.h"
#include "spectrum_extractor.h"

#pragma pack(push,1)
struct NoteRec
{
    uint8_t semitone;
    float velocity;    
};
#pragma pack(pop)

typedef struct NoteRec NoteRec;

NoteRec* match_freqs_and_notes(const FreqRec* freqs, const uint32_t freqs_n,
                               const float* semitones, const uint32_t semitones_n,
                               uint32_t *_notes_n);
#pragma once
#include "cmn_defs.h"
#include "midi_table_generator.h"

char* from_midi_table_to_midi_file(const MidiRec* midi_table, const uint32_t midi_table_recs_n, uint32_t *midi_file_size);
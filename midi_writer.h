#pragma once
#include "cmn_defs.h"
#include "notes_matcher.h"

char* generate_midi_file(const NoteRec* midi_table, const uint32_t midi_table_recs_n, uint32_t *midi_file_size);
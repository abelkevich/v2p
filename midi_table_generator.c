#include "midi_table_generator.h"

MidiRec* from_freqs_table_to_midi_table(const FreqRec* freqs_table, const uint32_t freqs_table_recs_n,
                                        const float* semitones_table, const uint32_t semitones_n,
                                        uint32_t *_midi_table_recs_n)
{
    MidiRec* midi_table = malloc(sizeof(MidiRec) * freqs_table_recs_n);

    if (!midi_table)
    {
        return NULL;
    }

    for (uint32_t rec_index = 0; rec_index<freqs_table_recs_n; rec_index++)
    {
        const FreqRec* freq_rec = freqs_table + rec_index;
        MidiRec* midi_rec = midi_table + rec_index;

        // filter hum
        if (freq_rec->freq < 40)
        {
            midi_rec->code = 0;
            midi_rec->velocity = 0;

            continue;
        }

        uint32_t nearest_semitone_index = 0;
        float smallest_freq_diff = FLT_MAX;

        for (uint32_t semitone_index = 0; semitone_index < semitones_n; semitone_index++)
        {
            float semitone_freq = semitones_table[semitone_index];
            float freq_diff = fabs(freq_rec->freq - semitone_freq);

            if (freq_diff < smallest_freq_diff)
            {
                smallest_freq_diff = freq_diff;
                nearest_semitone_index = semitone_index;
            }
        }

        midi_rec->code = nearest_semitone_index + 21;
        midi_rec->velocity = freq_rec->amp * 127;
    }

    *_midi_table_recs_n = freqs_table_recs_n; 
    return midi_table;
}
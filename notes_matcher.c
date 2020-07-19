#include "notes_matcher.h"

NoteRec* match_freqs_and_notes(const FreqRec* freqs, const uint32_t freqs_n,
                                      const float* semitones, const uint32_t semitones_n,
                                      uint32_t *_notes_n)
{
    NoteRec* notes = (NoteRec*) malloc(sizeof(NoteRec) * freqs_n);

    if (!notes)
    {
        return NULL;
    }

    for (uint32_t rec_ind = 0; rec_ind<freqs_n; rec_ind++)
    {
        const FreqRec* freq_rec = freqs + rec_ind;
        NoteRec* note_rec = notes + rec_ind;

        // filter hum
        if (freq_rec->freq < 40)
        {
            note_rec->semitone = 0;
            note_rec->velocity = 0;

            continue;
        }

        // determine what semitone is most similar to current freq
        uint32_t nearest_semitone_index = 0;
        float smallest_freq_diff = FLT_MAX;
 
        for (uint32_t semitone_index = 0; semitone_index < semitones_n; semitone_index++)
        {
            float semitone_freq = semitones[semitone_index];
            float freq_diff = fabs(freq_rec->freq - semitone_freq);

            if (freq_diff < smallest_freq_diff)
            {
                smallest_freq_diff = freq_diff;
                nearest_semitone_index = semitone_index;
            }
        }

        note_rec->semitone = nearest_semitone_index;
        note_rec->velocity = freq_rec->amp;
    }

    *_notes_n = freqs_n; 
    return notes;
}
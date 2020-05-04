#include "cmn_defs.h"
#include "ogg_opus_reader.h"
#include "freqs_table_generator.h"

#pragma pack(push,1)
struct MidiRec
{
    uint8_t code;
    uint8_t velocity; 
};
#pragma pack(pop)

typedef struct MidiRec MidiRec;

void normalize_amps(FreqRec* freqs_table, const uint32_t freqs_table_recs_n)
{
    float max_amp = 0;

    for (uint32_t rec_index = 0; rec_index<freqs_table_recs_n; rec_index++)
    {
        float rec_amp = freqs_table[rec_index].amp;
        
        if (max_amp < rec_amp)
        {
            max_amp = rec_amp;
        }
    }

    for (uint32_t rec_index = 0; rec_index<freqs_table_recs_n; rec_index++)
    {
        freqs_table[rec_index].amp /= max_amp;
    }
}

void print_freqs_table(const FreqRec* freqs_table, const uint32_t freqs_table_recs_n)
{
    printf("\nFreqs table: \n");
    for (uint32_t rec_index = 0; rec_index<freqs_table_recs_n; rec_index++)
    {
        FreqRec rec = freqs_table[rec_index];
        printf("freq: '%d' amp: '%f'\n", rec.freq, rec.amp);
    }
}

float* build_semitones_tune_table(const freq_t base_freq, const uint32_t semitones_n)
{
    const uint8_t semitone_mid_index = semitones_n / 2;
    float* semitones_freqs = (float*) malloc(sizeof(float) * semitones_n);

    if (!semitones_freqs)
    {
        return NULL;
    }

    for (uint8_t semitone_index = 0; semitone_index < semitones_n; semitone_index++)
    {
        float freq = base_freq * pow(2, (semitone_index - semitone_mid_index)/12.0);
        semitones_freqs[semitone_index] = freq;
    }

    return semitones_freqs;
}

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

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename.ogg\n", argv[0]);
        exit(1);
    }

    freq_t sample_rate = 0;
    smpn_t samples_n = 0;
    float* pcm_buffer = read_ogg_opus(argv[1], &sample_rate, &samples_n);
    
    if (!pcm_buffer)
    {
        fprintf(stderr, "Cannot read Ogg opus!");
        exit(1);
    }

    // need to add low-pass (<4kHz) and high-pass (>40Hz) filtering

    const ms_t division_time_ms = 10;
    const freq_t freqs_window = 4096;

    printf("division_time_ms: '%d'\n", division_time_ms);
    printf("freqs_window: '%d'\n", freqs_window);

    uint32_t freqs_table_recs_n = 0;
    FreqRec *freqs_table = get_freqs_table(freqs_window, division_time_ms, &freqs_table_recs_n, 
                                           pcm_buffer, sample_rate, samples_n);

    free(pcm_buffer);

    if (!freqs_table)
    {
        fprintf(stderr, "Cannot get freqs_table!");
        exit(1);
    }

    printf("freqs_table_recs_n: '%d'\n", freqs_table_recs_n);

    normalize_amps(freqs_table, freqs_table_recs_n);
    
    uint32_t semitones_n = 4 * 2 * 12; // 8 octaves
    float * semitones_table = build_semitones_tune_table(440, semitones_n);

    if (!semitones_table)
    {
        fprintf(stderr, "Cannot build semitones table!\n");
        exit(1);
    }

    uint32_t midi_table_recs_n = 0;
    MidiRec* midi_table = from_freqs_table_to_midi_table(freqs_table, freqs_table_recs_n, 
                                                         semitones_table, semitones_n,
                                                         &midi_table_recs_n);

    free(freqs_table);

    if (!midi_table)
    {
        fprintf(stderr, "Cannot build midi table!\n");
        exit(1);
    }

    printf("\nMidi table:\n");
    for (uint32_t i=0; i<midi_table_recs_n; i++)
    {
        MidiRec r = midi_table[i];
        printf("code: '%d' velocity: '%d'\n", r.code, r.velocity);
    }

    return 0;
}

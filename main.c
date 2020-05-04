#include "cmn_defs.h"
#include "ogg_opus_reader.h"
#include "freqs_table_generator.h"

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

    // need to add low-pass (<5kHz) and high-pass (>40Hz) filtering

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

    print_freqs_table(freqs_table, freqs_table_recs_n);

    free(freqs_table);

    return 0;
}

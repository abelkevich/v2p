#include "cmn_defs.h"
#include "ogg_opus_reader.h"
#include "freqs_table_generator.h"

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


    const ms_t division_time_ms = 10;
    const freq_t freqs_window = 4096;
    uint32_t freqs_table_recs_n = 0;

    printf("division_time_ms: '%d'\n", division_time_ms);
    printf("freqs_window: '%d'\n", freqs_window);

    FreqRec *freqs_table = get_freqs_table(freqs_window, division_time_ms, &freqs_table_recs_n, 
                                           pcm_buffer, sample_rate, samples_n);

    if (!freqs_table)
    {
        fprintf(stderr, "Cannot get freqs_table!");
        exit(1);
    }

    printf("freqs_table_recs_n: '%d'\n", freqs_table_recs_n);

    free(pcm_buffer);
    free(freqs_table);

    return 0;
}

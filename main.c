#include "tools/kiss_fftr.h"
#include <unistd.h>
#include <float.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <opus/opus.h>
#include <opus/opusfile.h>

typedef uint64_t smpn_t;
typedef int16_t smp_16_t;
typedef uint32_t freq_t;
typedef uint64_t ms_t;

const float pi = 3.14;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s filename.ogg\n", argv[0]);
        exit(1);
    }

    OggOpusFile* op_file = op_open_file(argv[1], NULL);

    if(!op_file) 
    {
        fprintf(stderr, "Input does not appear to be an Ogg bitstream.\n");
        exit(1);
    }

    if (op_channel_count(op_file, -1) != 1)
    {
        fprintf(stderr, "There is multichannel data\n");
        exit(1);
    }

    const freq_t sample_rate =  48000; // fixed due to opus codec limits
    const smpn_t samples_n = op_pcm_total(op_file, -1);

    // buffer holding only 10 ms
    const uint32_t pcm_buffer_len = sample_rate/10;

    printf("sample_rate: '%d'\n", sample_rate);
    printf("samples_n: '%d'\n", samples_n);
    printf("pcm_buffer_len: '%d'\n", pcm_buffer_len);

    float *pcm_buffer = malloc(sizeof(float) * pcm_buffer_len);

    if (!pcm_buffer)
    {
        exit(2);
    }

    kiss_fft_cpx *fft_out = malloc(sizeof(kiss_fft_cpx) * pcm_buffer_len);

    if (!fft_out)
    {
        exit(2);
    }

    kiss_fftr_cfg kiss_fft_state = kiss_fftr_alloc(pcm_buffer_len, 0, 0, 0);

    const uint32_t freqs_per_part = 5000;
    const uint32_t parts_n = samples_n / pcm_buffer_len;

    printf("freqs_per_part: '%d'\n", freqs_per_part);
    printf("parts_n: '%d'\n", parts_n);
    printf("memory usage to store freqs table: '%d'\n", parts_n*freqs_per_part*sizeof(uint32_t));
    

    uint8_t **freqs_by_parts_table = malloc(sizeof(uint8_t*) * parts_n);

    if (!freqs_by_parts_table)
    {
        exit(2);
    }

    for (uint32_t part_index = 0; part_index< parts_n; part_index++)
    {
        freqs_by_parts_table[part_index] = malloc(sizeof(uint8_t) * freqs_per_part);

        if (!freqs_by_parts_table[part_index])
        {
            exit(2);
        }
    } 

    for (uint32_t part_index = 0; part_index < parts_n; part_index++)
    {
        int samples_read = op_read_float(op_file, pcm_buffer, pcm_buffer_len, NULL);
        
        if (samples_read <= 0)
        {
            break;
        }

        kiss_fftr(kiss_fft_state, pcm_buffer, fft_out);
        
        for (int freq_ind=0; freq_ind < samples_read; freq_ind++)
        {
            float magnitude = sqrt(fft_out[freq_ind].r*fft_out[freq_ind].r + 
                                   fft_out[freq_ind].i*fft_out[freq_ind].i);

            freqs_by_parts_table[part_index][freq_ind] = UINT8_MAX / magnitude;
        }
    }

    free(kiss_fft_state);
    op_free(op_file);

    return 0;
}

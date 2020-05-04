#include "tools/kiss_fftr.h"
#include <unistd.h>
#include <float.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cmn_defs.h"
#include "ogg_opus_reader.h"


#pragma pack(push,1)
typedef struct FreqRec
{
    uint16_t freq;
    float amp;
} FreqRec;
#pragma pack(pop)

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


    const ms_t part_len_ms = 10;
    const smpn_t samples_per_part = sample_rate/1000 * part_len_ms;
    const uint32_t parts_n = samples_n / samples_per_part;

    const freq_t fft_freqs = 4096;
    const uint32_t fft_buffer_len = fft_freqs * 2;


    printf("part_len_ms: '%d'\n", part_len_ms);
    printf("samples_per_part: '%d'\n", samples_per_part);
    printf("parts_n: '%d'\n", parts_n);
    printf("fft_freqs: '%d'\n", fft_freqs);
    printf("fft_buffer_len: '%d'\n", fft_buffer_len);
    printf("memory usage to store freqs table: '%d'B\n", parts_n * sizeof(FreqRec));

    kiss_fft_cpx *fft_buffer = malloc(sizeof(kiss_fft_cpx) * fft_buffer_len);

    if (!fft_buffer)
    {
        exit(2);
    }

    kiss_fftr_cfg kiss_fft_state = kiss_fftr_alloc(fft_buffer_len, 0, 0, 0);
    
    FreqRec *maxfreqs_in_parts = malloc(sizeof(FreqRec) * parts_n);

    if (!maxfreqs_in_parts)
    {
        exit(2);
    }

    for (uint32_t part_index = 0; part_index < parts_n; part_index++)
    {
        // is out of range (samples_per_part <<< fft_buffer_len)
        // need to move check before loop begins
        if (part_index * samples_per_part + fft_buffer_len > samples_n)
        {
            break;
        }

        float *pcm_buffer_offset = pcm_buffer + (part_index * samples_per_part);

        kiss_fftr(kiss_fft_state, pcm_buffer_offset, fft_buffer);
        
        FreqRec max_freq_rec = {0, 0.0};
        for (uint32_t freq_index=0; freq_index < fft_buffer_len / 2; freq_index++)
        {
            float r = fft_buffer[freq_index].r;
            float i = fft_buffer[freq_index].i;

            float amp = sqrtf(r*r + i*i);

            if (amp > max_freq_rec.amp)
            {
                max_freq_rec.amp = amp;
                max_freq_rec.freq = freq_index;
            }
        }

        maxfreqs_in_parts[part_index] = max_freq_rec;
        //printf("freq: '%d' amp: '%1.3f'\n", max_freq_rec.freq, max_freq_rec.amp);
    }

    free(pcm_buffer);
    free(fft_buffer);
    free(maxfreqs_in_parts);
    free(kiss_fft_state);

    return 0;
}

#include "tools/kiss_fftr.h"
#include "cmn_defs.h"
#include "freqs_table_generator.h"

FreqRec* get_freqs_table(const freq_t freq_window, const ms_t division_time_ms, uint32_t *_recs_n,
                         const float* pcm_buffer, const smpn_t sample_rate, const smpn_t samples_n)
{
    const smpn_t samples_per_part = (sample_rate/1000) * division_time_ms;
    const uint32_t recs_n = samples_n / samples_per_part;
    
    const uint32_t fft_buffer_len = freq_window * 2;
 
    kiss_fft_cpx *fft_buffer = malloc(sizeof(kiss_fft_cpx) * fft_buffer_len);

    if (!fft_buffer)
    {
        fprintf(stderr, "Cannot allocate memory for fft_buffer!\n");
        return NULL;
    }

    kiss_fftr_cfg kiss_fft_state = kiss_fftr_alloc(fft_buffer_len, 0, 0, 0);
    
    FreqRec *freqs_table = malloc(sizeof(FreqRec) * recs_n);

    if (!freqs_table)
    {
        fprintf(stderr, "Cannot allocate memory for freqs_by_parts!\n");
        return NULL;
    }

    for (uint32_t part_index = 0; part_index < recs_n; part_index++)
    {
        // is out of range (samples_per_part <<< fft_buffer_len)
        // need to move check before loop begins
        if (part_index * samples_per_part + fft_buffer_len > samples_n)
        {
            break;
        }

        const float *pcm_buffer_offset = pcm_buffer + (part_index * samples_per_part);

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

        freqs_table[part_index] = max_freq_rec;
    }

    free(fft_buffer);
    free(kiss_fft_state);

    *_recs_n = recs_n;
    return freqs_table;
}
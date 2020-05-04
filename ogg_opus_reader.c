#include "ogg_opus_reader.h"
#include <stdlib.h>
#include <opus/opus.h>
#include <opus/opusfile.h>

float* read_ogg_opus(const char* filename, freq_t *_sample_rate, smpn_t *_samples_n)
{
    OggOpusFile* op_file = op_open_file(filename, NULL);

    if(!op_file) 
    {
        fprintf(stderr, "Cannot recognize ogg bitstream!\n");
        return NULL;
    }

    if (op_channel_count(op_file, -1) != 1)
    {
        fprintf(stderr, "There is multichannel data!\n");
        return NULL;
    }

    const freq_t sample_rate =  48000; // fixed due to opus codec limits
    const smpn_t samples_n = op_pcm_total(op_file, -1);

    printf("sample_rate: '%d'\n", sample_rate);
    printf("samples_n: '%d'\n", samples_n);

    if (samples_n > sample_rate * 90)
    {
        fprintf(stderr, "Ogg contents is more than 90 seconds\n");
        return NULL;
    }
    
    float *pcm_buffer = malloc(sizeof(float) * samples_n);

    if (!pcm_buffer)
    {
        fprintf(stderr, "Failed to allocate data for pcm_buffer\n");
        return NULL;
    }

    // read ogg
    {
        int offset = 0;
        while(1)
        {
            int samples_read = op_read_float(op_file, pcm_buffer + offset, 512, NULL);
            
            if (samples_read <= 0)
            {
                break;
            }

            offset += samples_read;
        }
    }

    op_free(op_file);

    *_sample_rate = sample_rate;
    *_samples_n = samples_n;

    return pcm_buffer;
}
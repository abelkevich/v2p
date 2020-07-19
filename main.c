#include "cmn_defs.h"
#include "ogg_opus_reader.h"
#include "spectrum_extractor.h"
#include "notes_matcher.h"
#include "midi_writer.h"

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

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s input.ogg out.mid\n", argv[0]);
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

    uint32_t freqs_n = 0;
    FreqRec *freqs = break_into_parts_and_find_lead_freqs(freqs_window, division_time_ms, &freqs_n, 
                                     pcm_buffer, sample_rate, samples_n);

    free(pcm_buffer);

    if (!freqs)
    {
        fprintf(stderr, "Cannot get freqs_table!");
        exit(1);
    }

    printf("freqs_table_recs_n: '%d'\n", freqs_n);

    normalize_amps(freqs, freqs_n);
    
    uint32_t semitones_n = 4 * 2 * 12; // 8 octaves
    float * semitones_table = build_semitones_tune_table(440, semitones_n);

    if (!semitones_table)
    {
        fprintf(stderr, "Cannot build semitones table!\n");
        exit(1);
    }

    uint32_t notes_n = 0;
    NoteRec* notes = match_freqs_and_notes(freqs, freqs_n, 
                                           semitones_table, semitones_n,
                                           &notes_n);

    free(freqs);

    if (!notes)
    {
        fprintf(stderr, "Cannot build midi table!\n");
        exit(1);
    }

    /*
    printf("\nMidi table:\n");
    for (uint32_t i=0; i<midi_table_recs_n; i++)
    {
        MidiRec r = midi_table[i];
        printf("code: '%d' velocity: '%d'\n", r.code, r.velocity);
    }
    */

    uint32_t midi_file_size = 0;
    char* midi_file_data = generate_midi_file(notes, notes_n, &midi_file_size);

    free(notes);

    if (!midi_file_data)
    {
        fprintf(stderr, "Cannot build midi file!\n");
        exit(1);
    }

    FILE* midi_file = fopen(argv[2], "wb");

    if (!midi_file)
    {
        fprintf(stderr, "Cannot open midi file for writing!\n");
        exit(1);
    }

    if (fwrite(midi_file_data, 1, midi_file_size, midi_file) != midi_file_size)
    {
        fprintf(stderr, "Cannot write midi data!\n");
        exit(1);
    }

    fclose(midi_file);

    free(midi_file_data);

    return 0;
}

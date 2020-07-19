#include "midi_writer.h"

#pragma pack(push,1)
struct MidiHeader
{
    char mthd[4];
    uint32_t header_len;
    uint16_t format;
    uint16_t n;
    uint16_t div;
};
#pragma pack(pop)

#pragma pack(push,1)
struct TrackHeader
{
    char mtrk[4];
    uint32_t data_len;
};
#pragma pack(pop)

#pragma pack(push,1)
struct EventNoteOnOff
{
    char on_off_channel;
    char key;
    char velocity;
};
#pragma pack(pop)

typedef struct MidiHeader MidiHeader;
typedef struct TrackHeader TrackHeader;
typedef struct EventNoteOnOff EventNoteOnOff;

const uint8_t c_midi_header_size = 14;
const uint8_t c_track_header_size = 8;

static uint16_t Reverse16(uint16_t value)
{
    return (((value & 0x00FF) << 8) |
            ((value & 0xFF00) >> 8));
}
    
static uint32_t Reverse32(uint32_t value) 
{
    return (((value & 0x000000FF) << 24) |
            ((value & 0x0000FF00) <<  8) |
            ((value & 0x00FF0000) >>  8) |
            ((value & 0xFF000000) >> 24));
}

static void write_delta_time(char** data_offset, uint8_t ticks)
{
    **data_offset = ticks; // delta time
    *data_offset += 1;
}

static void write_note_on_off_status(char** data_offset, const uint8_t midi_note, const uint8_t velocity, const uint8_t is_on)
{
    static char c_note_on =  0b10010000 | 0b0000; // add channel as last 4 bits
    static char c_note_off = 0b10000000 | 0b0000;

    EventNoteOnOff event = {0, 0, 0};
    event.on_off_channel = is_on ? c_note_on : c_note_off;
    event.key = midi_note;
    event.velocity = velocity;

    memcpy(*data_offset, &event, sizeof(EventNoteOnOff));

    *data_offset += sizeof(EventNoteOnOff);
}

char* generate_midi_file(const NoteRec* notes, const uint32_t notes_n, uint32_t *midi_file_size)
{
    char* const data = (char*) malloc(8 * notes_n + sizeof(MidiHeader) + sizeof(TrackHeader));
    char* data_offset = data;

    {
        MidiHeader midi_header;
        strcpy(midi_header.mthd, "MThd");
        midi_header.header_len = Reverse32(6);
        midi_header.format = 0;
        midi_header.n = Reverse16(1);
        midi_header.div = Reverse16(64);

        memcpy(data_offset, &midi_header, sizeof(MidiHeader));
        data_offset += sizeof(MidiHeader);
    }

    {
        TrackHeader track_header;
        strcpy(track_header.mtrk, "MTrk");
        track_header.data_len = Reverse32(notes_n * 2 * 4);

        memcpy(data_offset, &track_header, sizeof(TrackHeader));
        data_offset += sizeof(TrackHeader);
    }

    for (uint32_t note_ind = 0; note_ind < notes_n; note_ind++)
    {
        uint8_t midi_note = notes[note_ind].semitone + 21;
        uint8_t velocity = notes[note_ind].velocity * 127;

        write_delta_time(&data_offset, 0);
        write_note_on_off_status(&data_offset, midi_note, velocity, 1);
        write_delta_time(&data_offset, 32);
        write_note_on_off_status(&data_offset, midi_note, velocity, 0);
    }

    *midi_file_size = (uint32_t) (data_offset - data);
    return data;
}
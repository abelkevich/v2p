#include "midi_writer.h"

#pragma pack(push,1)
struct MidiHeader
{
    char mthd[4];
    char header_len[4];
    char format[2];
    char n[2];
    char div[2];
};
#pragma pack(pop)

#pragma pack(push,1)
struct TrackHeader
{
    char mtrk[4];
    char data_len[4];
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

char c_note_on = 0b1001;
char c_note_off = 0b1000;

char* generate_midi_file(const NoteRec* notes, const uint32_t notes_n, uint32_t *midi_file_size)
{
    char* const data = (char*) malloc(3000);
    char* data_offset = data;

    static const MidiHeader midi_header = {{'M', 'T', 'h', 'd'}, {0x00, 0x00, 0x00, 0x06}, 
                                            {0x00, 0x00}, {0x00, 0x01}, {0xE7, 0x28}};
    memcpy(data_offset, &midi_header, sizeof(MidiHeader));
    data_offset += sizeof(MidiHeader);
    
    static const TrackHeader track_header = {{'M', 'T', 'r', 'k'}, {0x00, 0x00, 0x0B, 0x88}};
    memcpy(data_offset, &track_header, sizeof(TrackHeader));
    data_offset += sizeof(TrackHeader);

    EventNoteOnOff event_note_on_off = {0, 0, 0};

    for (uint32_t midi_table_ind = 0; midi_table_ind < notes_n; midi_table_ind++)
    {
        event_note_on_off.key = notes[midi_table_ind].semitone + 21;
        event_note_on_off.velocity = notes[midi_table_ind].velocity * 127;

        event_note_on_off.on_off_channel = c_note_on << 4 | 0b0000;
        
        *data_offset = 0b00000010; // delta time
        data_offset += 1;

        memcpy(data_offset, &event_note_on_off, sizeof(EventNoteOnOff));
        data_offset += sizeof(EventNoteOnOff);

        event_note_on_off.on_off_channel = c_note_off << 4 | 0b0000;
        
        *data_offset = 0b00000010; // delta time
        data_offset += 1;

        memcpy(data_offset, &event_note_on_off, sizeof(EventNoteOnOff));
        data_offset += sizeof(EventNoteOnOff);
    }

    *midi_file_size = (uint32_t) (data_offset - data);
    return data;
}
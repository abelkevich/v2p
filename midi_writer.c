#include "cmn_defs.h"
#include "midi_table_generator.h"

#pragma pack(push,1)
struct HeaderChunk
{
    char mthd[4];
    uint32_t header_len;
    uint16_t format;
    uint16_t n;
    int16_t div;
};
#pragma pack(pop)

#pragma pack(push,1)
struct TrackChunk
{
    char mtrk[4];
    uint32_t data_len;
    void *data;
};
#pragma pack(pop)


#pragma pack(push,1)
struct TrackChunk
{
    char mtrk[4];
    uint32_t data_len;
    void *data;
};
#pragma pack(pop)

typedef struct HeaderChunk HeaderChunk;
typedef struct TrackChunk TrackChunk;

static const HeaderChunk c_header_chunk = {{'M', 'T', 'h', 'd'}, 6, 0, 1, -1};


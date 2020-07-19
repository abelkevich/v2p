KISSFFT_DIR = third_party/kissfft
OPUS_DIR = /usr/include/opus

CFALGS = -O0 -g

PROJ_NAME = v2p

.phony: all clean

all: $(PROJ_NAME)

clean:
	rm -rf *.o
	rm -rf $(PROJ_NAME)

kiss_fftr.o: $(KISSFFT_DIR)/tools/kiss_fftr.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

kiss_fft.o: $(KISSFFT_DIR)/kiss_fft.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

main.o: main.c cmn_defs.h spectrum_extractor.h ogg_opus_reader.h notes_matcher.h midi_writer.h
	$(CC) $(CFLAGS) -c $< -o $@

spectrum_extractor.o: spectrum_extractor.c spectrum_extractor.h cmn_defs.h
	$(CC) $(CFALGS) -I$(KISSFFT_DIR) -c $< -o $@

notes_matcher.o: notes_matcher.c notes_matcher.h spectrum_extractor.h cmn_defs.h
	$(CC) $(CFALGS) -c $< -o $@

midi_writer.o: midi_writer.c notes_matcher.h cmn_defs.h
	$(CC) $(CFALGS) -c $< -o $@

ogg_opus_reader.o: ogg_opus_reader.c ogg_opus_reader.h cmn_defs.h
	$(CC) $(CFALGS) -I$(OPUS_DIR) -c $< -o $@

$(PROJ_NAME): main.o kiss_fftr.o kiss_fft.o ogg_opus_reader.o spectrum_extractor.o notes_matcher.o midi_writer.o
	$(CC) -o $@ $^ -lopusfile -lm
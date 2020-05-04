KISSFFT_DIR = third_party/kissfft
OPUS_DIR = /usr/include/opus

CFALGS = -O0 -g

.phony: all clear

all: pitch_analyzer

clear:
	rm -rf *.o
	rm -rf pitch_analyzer

kiss_fftr.o: $(KISSFFT_DIR)/tools/kiss_fftr.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

kiss_fft.o: $(KISSFFT_DIR)/kiss_fft.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

ogg_opus_reader.o: ogg_opus_reader.c ogg_opus_reader.h
	$(CC) $(CFALGS) -I$(OPUS_DIR) -c $< -o $@

pitch_analyzer: main.o kiss_fftr.o kiss_fft.o ogg_opus_reader.o
	$(CC) -o $@ $^ -lopusfile -lm
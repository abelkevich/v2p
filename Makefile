KISSFFT_DIR = third_party/kissfft
OPUS_DIR = /usr/include/opus

CFALGS = -O0 -g

all: pitch_analyzer

clear:
	rm -rf *.o
	rm -rf pitch_analyzer

kiss_fftr.o: $(KISSFFT_DIR)/tools/kiss_fftr.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

kiss_fft.o: $(KISSFFT_DIR)/kiss_fft.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -I$(KISSFFT_DIR) -I$(OPUS_DIR) -c $< -o $@

pitch_analyzer: main.o kiss_fftr.o kiss_fft.o
	$(CC) -o $@ $^ -lopusfile -lm
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

main.o: main.c cmn_defs.h freqs_table_generator.h ogg_opus_reader.h
	$(CC) $(CFLAGS) -c $< -o $@

freqs_table_generator.o: freqs_table_generator.c freqs_table_generator.h cmn_defs.h
	$(CC) $(CFALGS) -I$(KISSFFT_DIR) -c $< -o $@

ogg_opus_reader.o: ogg_opus_reader.c ogg_opus_reader.h cmn_defs.h
	$(CC) $(CFALGS) -I$(OPUS_DIR) -c $< -o $@

$(PROJ_NAME): main.o kiss_fftr.o kiss_fft.o ogg_opus_reader.o freqs_table_generator.o
	$(CC) -o $@ $^ -lopusfile -lm
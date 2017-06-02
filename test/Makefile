all : file_loc gpsupdate

CC = arm-linux-gnueabi-gcc

file_loc : file_loc.c
	$(CC) -I ../include -o $@ $<

gpsupdate : gpsupdate.c
	$(CC) -I ../include -o $@ $<

EXEC=WORM

CC=gcc
LD=ld
CFLAGS=-I/usr/include/fuse -D_FILE_OFFSET_BITS=64
LDFLAGS=-lfuse

syno:CC=/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-gcc
syno:LD=/usr/local/arm-marvell-linux-gnueabi/bin/arm-marvell-linux-gnueabi-ld
syno:CFLAGS+= -I/usr/local/arm-marvell-linux-gnueabi/arm-marvell-linux-gnueabi/libc/include
syno:LDFLAGS += -L/usr/local/arm-marvell-linux-gnueabi/arm-marvell-linux-gnueabi/libc/lib

all:$(EXEC)

syno:$(EXEC)

Debug:$(EXEC)

Release:$(EXEC)

$(EXEC):main.o Attribute.o Directory.o File.o Link.o Logger.o Retention.o Shared.o Utils.o inifile.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

cleanRelease:clean

clean :
	rm *.o $(EXEC)

/etc/WORM.conf:
	cp WORM.conf /etc

install:$(EXEC) /etc/WORM.conf
	cp $(EXEC) /usr/local/bin
	cp ParseAudit.pl /usr/local/bin

uninstall:
	rm /usr/local/bin/$(EXEC) /etc/WORM.conf /usr/local/bin/ParseAudit.pl

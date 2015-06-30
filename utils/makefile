CC=gcc
LD=ld
CFLAGS=
LDFLAGS=


all:getretention setretention


getretention:getretention.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
 
setretention:setretention.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean :
	rm *.o getretention setretention

install:$(EXEC)
	cp getretention setretention /usr/local/bin
	
uninstall:
	rm /usr/local/bin/getretention /usr/local/bin/setretention



































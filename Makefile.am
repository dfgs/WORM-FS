AUTOMAKE_OPTIONS = foreign
wormconfdir=/etc
wormconf_DATA = worm.conf worm.service
bin_PROGRAMS = worm
worm_SOURCES = main.c attribute.c directory.c file.c inifile.c link.c logger.c retention.c shared.c utils.c attribute.h directory.h file.h inifile.h link.h logger.h retention.h shared.h utils.h worm.conf worm.service
install-service:
	cp worm.service /usr/lib/systemd/system/

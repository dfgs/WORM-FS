AUTOMAKE_OPTIONS = foreign
wormconfdir=/etc
wormconf_DATA = worm.conf
wormrotatedir=/etc/logrotate.d
wormrotate_DATA = wormrotate
bin_PROGRAMS = worm
worm_SOURCES = main.c attribute.c directory.c file.c link.c logger.c retention.c shared.c utils.c loop.c context.c filter.c config.c attribute.h directory.h file.h link.h logger.h retention.h shared.h utils.h loop.h context.h filter.h config.h worm.conf worm.service wormrotate
install-service:
	cp worm.service /usr/lib/systemd/system/

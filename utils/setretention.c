#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <dirent.h>
#include <string.h>

int main(int argc, char **argv)
{
	unsigned short retention;
    int status;
	char *path;
	
	if (argc<3)
	{
		printf("Bad arguments, expected syntax: setretention <path> <retention days>\n");
		return 0;
	}
 
	status=sscanf(argv[2],"%hu",&retention);
	if (status==EOF)
	{
		printf("Bad arguments, expected syntax: setretention <path> <retention days>\n");
		return 0;
	}
	
	path=argv[1];
	status = lsetxattr(path, "user.Retention", &retention, sizeof(retention),0);
	if (status<0)
	{
		printf("Cannot set retention: %s\n",strerror(errno));
	}
	else
	{
		printf("Retention set\n");
	}


	return 0;
}


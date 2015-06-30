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
#include <inttypes.h>

int main(int argc, char **argv)
{
	unsigned short retention;
	int status;
	time_t expiration;
	char *path;
	time_t now;
	int64_t value;
	char* timeString;

	if (argc<2)
	{
		printf("Bad arguments, expected syntax: getretention <path>\n");
		return 0;
	}


	path=argv[1];
	status = lgetxattr(path, "user.Retention", &retention, sizeof(retention));
	if (status<0)
	{
		printf("Cannot get retention: %s\n",strerror(errno));
	}
	else
	{
		printf("Retention is %hu day(s)\n",retention);
	}

	status = lgetxattr(path, "user.ExpirationDate", &value, sizeof(value));
	if (status<0)
	{
		printf("Cannot get expiration date: %s\n",strerror(errno));
	}
	else
	{

		//printf("Value is : %" PRId64,value);

		expiration=value;   // be sure to read 64 bits
		timeString=ctime(&expiration);
		if (timeString==NULL) printf("Expiration date is (NULL)\n");
		else printf("Expiration date is %s\n",timeString);
		
		now=time(NULL);
				
		if (now>expiration)
		{
			printf("Media is expired\n");
		}
		else
		{
			printf("Media is not expired\n");
		}
	}

	
	return 0;
}


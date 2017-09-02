#include <string.h>
#include <fcntl.h>
#include <fuse.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#include <log.h>
#include "logger.h"
#include "retention.h"
#include "context.h"
#include "config.h"

const char* DEBUG="DEBUG";
const char* INFO="INFO";
const char* WARN="WARN";
const char* ERROR="ERROR";

const char* SUCCESS="SUCCESS";
const char* FAILURE="FAILURE";
const char* OK="OK";
const char* NOK="NOK";
const char* NOTEXPIRED="NOTEXPIRED";

const char* CREATE="CREATE";
const char* DELETE="DELETE";
const char* UPDATE="UPDATE";

const char* cFILE="FILE";
const char* DIRECTORY="DIRECTORY";
const char* EXPIRATION="EXPIRATION";
const char* LOCK="LOCK";
const char* RETENTION="RETENTION";
const char* OWNER="OWNER";
const char* MODE="MODE";
const char* TIME="TIME";
const char* LOCATION="LOCATION";
const char* ATTRIBUTE="ATTRIBUTE";

static Logger logger= {.auditQueue=(mqd_t)-1};

 
void (*writeAudit)(const char*,const char*,const char* ,const char* ,const char*);


void logger_enter(const char* funcName,const char* path)
{
	logger_log(funcName,path,DEBUG,"Entering..." );
}

void logger_log(const char* funcName,const char* path,const char* errorLevel,const char *format, ...)
{
	char buffer[1024];
	va_list ap;

	va_start(ap, format);
	vsnprintf(buffer,1024, format, ap);
	va_end(ap);
	
	log_write(logger.logFile,"%i|%s|%s|%s|%s",config.ID,errorLevel,funcName,path,buffer);
}

int logger_errno(const char* funcName,const char* path,const char* message)
{
    logger_log(funcName,path, ERROR,"%s: %s", message, strerror(errno));
	return -errno;
}




void writeAuditToNull(const char *action,const char *entity,const char* result,const char* path,const char *value)
{

}

void writeAuditToQueue(const char *action,const char *entity,const char* result,const char* path,const char *value)
{
	log_write(logger.logFile,"TEST QUEUE");
	if (logger.auditQueue == (mqd_t) -1) 
	{
		log_write(logger.logFile,"QUEUE invalid");
	}
	mq_send(logger.auditQueue, "TEST", 5, 1);
}

void writeAuditToFile(const char *action,const char *entity,const char* result,const char* path,const char *value)
{
	struct fuse_context *fuseContext;

	fuseContext=fuse_get_context();
	
	log_write(logger.auditFile,"%i|%s|%s|%s|%s|%s|%i|%i",config.ID,action,entity,result,path,value,fuseContext->uid,fuseContext->gid);
}


void logger_auditSuccess(const char *action,const char *entity,const char* path,const char *format, ...)
{
	va_list ap;
	char message[PATH_MAX];

 	va_start(ap, format);
	vsnprintf(message,PATH_MAX,format,ap);
	va_end(ap);

	writeAudit(action,entity,SUCCESS,path,message);
}
void logger_auditFailure(const char *action,const char *entity,const char* path,const char *format, ...)
{
	va_list ap;
	char message[PATH_MAX];

 	va_start(ap, format);
	vsnprintf(message,PATH_MAX,format,ap);
	va_end(ap);

	writeAudit(action,entity,FAILURE,path,message);
}

int logger_init()
{
	printf("Initializing logs...\n");
	logger.logFile=log_open("/var/log/worm.log",SIGUSR1);
	if (logger.logFile==NULL) 
	{
		fprintf(stderr,"Failed to initialize log file\n");
		return -1;
	}
	
	printf("Initializing callbacks...\n");
	switch(config.auditMode)
	{
		case 1:
			logger.auditFile=log_open("/var/log/worm_audit.log",SIGUSR2);
			if (logger.auditFile==NULL) 
			{
				fprintf(stderr,"Failed to initialize audit file");
				return -1;
			}
			writeAudit=&writeAuditToFile;
			break;
		case 2:
			logger.auditQueue = mq_open("/worm_audit", O_WRONLY | O_CREAT | O_NONBLOCK, 0600, NULL);
			if (logger.auditQueue == (mqd_t) -1) 
			{
				fprintf(stderr,"Failed to initialize audit queue");
				return -1;
			}
			writeAudit=&writeAuditToQueue;
			break;
		default:
			writeAudit=&writeAuditToNull;
			break;
	}

	return 0;
	 
 }

void logger_free()
{
	if (logger.logFile!=NULL) log_close(logger.logFile);
	if (logger.auditFile!=NULL) log_close(logger.auditFile);
	if (logger.auditQueue!=(mqd_t)-1)
	{
		mq_close(logger.auditQueue);
		mq_unlink ("/worm_audit");
	}
}

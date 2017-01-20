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
#include "logger.h"
#include <limits.h>
#include "retention.h"
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

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

unsigned char ID=1;
int maxLogFileLines=100;
int maxAuditFileLines=100;
int writeAuditFiles=0;
static int currentLogFileLines;
static int currentAuditFileLines;
int lockDelay=300;
int autoLock=300;
char *auditFilePath;
char auditFileName[PATH_MAX];
char logFileName[PATH_MAX];

	
FILE *logFile;
FILE *auditFile;
pthread_mutex_t log_mutex;
pthread_mutex_t audit_mutex;

void sig_handler(int signo)
{
	writeLog(DEBUG,"sig_handler","","Received signal %i",signo);
	if (signo==SIGUSR1)
	{
		pthread_mutex_lock(&audit_mutex);
		closeAudit();
		openAudit();
		pthread_mutex_unlock(&audit_mutex);
	}
	/*else if (signo==SIGUSR2)
	{
		pthread_mutex_lock(&log_mutex);
		closeLog();
		openLog();
		pthread_mutex_unlock(&log_mutex);
	}*/

}


void initLog()
{

	pthread_mutex_init(&log_mutex,NULL);
	pthread_mutex_init(&audit_mutex,NULL);


 	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
	{
		perror("Failed to register signal USR1");
		exit(EXIT_FAILURE);
	}
 	/*if (signal(SIGUSR2, sig_handler) == SIG_ERR)
	{
		perror("Failed to register signal USR2");
		exit(EXIT_FAILURE);
	}*/
}

void disposeLog()
{
	pthread_mutex_destroy(&log_mutex);
	pthread_mutex_destroy(&audit_mutex);
}

void openLog()
{
    int result;

    if (fileExists(logFileName)==1) renameLog();

    currentLogFileLines=0;
	logFile = fopen(logFileName, "w");
	if (logFile == NULL)
	{
		perror("Failed to create log file");
		exit(EXIT_FAILURE);
	}
 	result=setvbuf(logFile, NULL, _IOLBF, 0);//*/
    if (result!=0)
    {
 		perror("Failed to set log file buffer");
   }
}


void closeLog()
{
	fclose(logFile);
    renameLog();
}

void renameLog()
{
	long long now;
	char newFileName[PATH_MAX];

	now=time(NULL);
	sprintf(newFileName,"/var/log/WORM-%llu.log",now);

	rename(logFileName,newFileName);

}

void logEnter(const char* funcName,const char* path)
{
	writeLog(funcName,path,DEBUG,"Entering..." );
}
/*void LogExit(const char *Function)
{
	writeLog(DEBUG,"Exit function %s",Function);
}*/

void writeLog(const char* funcName,const char* path,const char *errorLevel,const char *format, ...)
{
	va_list ap;
	time_t now;
	char buf[256];


	pthread_mutex_lock(&log_mutex);

	now=time(NULL);
    strcpy(buf,ctime(&now));
    buf[strlen(buf)-1]='\0';

	fprintf(logFile,"%s|%hhu|%s|%s|",buf,ID,errorLevel,funcName);

	va_start(ap, format);
	vfprintf(logFile, format, ap);
	va_end(ap);

	fprintf(logFile,"|%s\n",path);
	fflush(logFile);


    currentLogFileLines++;
    if (currentLogFileLines==maxLogFileLines)
    {
        closeLog();
        openLog();
    }

	pthread_mutex_unlock(&log_mutex);

}

int writeErrorNumber(const char* funcName,const char* path)
{
    writeLog(funcName,path, ERROR,"Returned error is (%i: %s)", errno, strerror(errno));
	return -errno;
}


void openAudit()
{
    int result;

    if (!writeAuditFiles) return;

	if (fileExists(auditFileName)==1) renameAudit();

    currentAuditFileLines=0;
	auditFile = fopen(auditFileName, "w");
	if (logFile == NULL)
	{
		perror("Failed to create audit file");
		exit(EXIT_FAILURE);
	}
 	result=setvbuf(auditFile, NULL, _IOLBF, 0);//*/
    if (result!=0)
    {
        perror("Failed to set audit file buffer");
    }
}

void renameAudit()
{
	long long now;
	char newFileName[PATH_MAX];

	now=time(NULL);
	sprintf(newFileName,"%s/WORM_Audit-%llu.log",auditFilePath, now);

	rename(auditFileName,newFileName);
}

void closeAudit()
{
    if (!writeAuditFiles) return;

	fclose(auditFile);
    renameAudit();

}

void writeAudit(const char *action,const char *entity,const char* result,const char* path,const char *value)
{
	long long now;
	struct fuse_context *context;

	if (!writeAuditFiles) return;
	pthread_mutex_lock(&audit_mutex);
	context=fuse_get_context();
	now=time(NULL);

	fprintf(auditFile,"%llu|%hhu|%s|%s|%s|%s|%s|%i|%i\n",now,ID,action,entity,result,path,value,context->uid,context->gid);
	fflush(auditFile);

	currentAuditFileLines++;
	if (currentAuditFileLines==maxAuditFileLines)
	{
		closeAudit();
		openAudit();
	}
	pthread_mutex_unlock(&audit_mutex);
	
	
}

void auditSuccess(const char *action,const char *entity,const char* path,const char *format, ...)
{
	va_list ap;
	char message[PATH_MAX];

    if (writeAuditFiles==0) return;

	va_start(ap, format);
	vsprintf(message,format,ap);
	va_end(ap);

	writeAudit(action,entity,SUCCESS,path,message);
}
void auditFailure(const char *action,const char *entity,const char* path,const char *format, ...)
{
	va_list ap;
	char message[PATH_MAX];

    if (writeAuditFiles==0) return;

	va_start(ap, format);
	vsprintf(message,format,ap);
	va_end(ap);

	writeAudit(action,entity,FAILURE,path,message);
}

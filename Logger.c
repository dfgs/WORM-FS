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
#include "Logger.h"
#include <limits.h>
#include "Retention.h"
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
const char* RETENTION="RETENTION";
const char* OWNER="OWNER";
const char* MODE="MODE";
const char* TIME="TIME";
const char* LOCATION="LOCATION";
const char* ATTRIBUTE="ATTRIBUTE";

unsigned char ID=1;
int MaxLogFileLines=100;
int MaxAuditFileLines=100;
int WriteAuditFiles=0;
static int currentLogFileLines;
static int currentAuditFileLines;

FILE *logFile;
FILE *auditFile;
pthread_mutex_t log_mutex;
pthread_mutex_t audit_mutex;

void sig_handler(int signo)
{
	WriteLog(DEBUG,"Received signal %i",signo);
	if (signo==SIGUSR1)
	{
		pthread_mutex_lock(&audit_mutex);
		CloseAudit();
		OpenAudit();
		pthread_mutex_unlock(&audit_mutex);
	}
	/*else if (signo==SIGUSR2)
	{
		pthread_mutex_lock(&log_mutex);
		CloseLog();
		OpenLog();
		pthread_mutex_unlock(&log_mutex);
	}*/

}


void InitLog()
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

void DisposeLog()
{
	pthread_mutex_destroy(&log_mutex);
	pthread_mutex_destroy(&audit_mutex);
}

void OpenLog()
{
    if (FileExists("/var/log/WORM.log")==1) RenameLog();

    currentLogFileLines=0;
	logFile = fopen("/var/log/WORM.log", "w");
	if (logFile == NULL)
	{
		perror("Failed to create log file");
		exit(EXIT_FAILURE);
	}
 	setvbuf(logFile, NULL, _IOLBF, 0);//*/
 	//WriteLog(DEBUG,"Default retention=%i",DefaultRetention);

}


void CloseLog()
{
	fclose(logFile);
    RenameLog();
}

void RenameLog()
{
	long long now;
	char fileName[PATH_MAX];

	now=time(NULL);
	sprintf(fileName,"/var/log/WORM-%llu.log",now);

	rename("/var/log/WORM.log",fileName);

}

void LogEnter(const char *FuncName)
{
	WriteLog(DEBUG,"Enter function %s",FuncName);
}
/*void LogExit(const char *Function)
{
	WriteLog(DEBUG,"Exit function %s",Function);
}*/

void WriteLog(const char *ErrorLevel,const char *format, ...)
{
	va_list ap;
	time_t now;
	char buf[256];


	pthread_mutex_lock(&log_mutex);

	now=time(NULL);
    strcpy(buf,ctime(&now));
    buf[strlen(buf)-1]='\0';

	fprintf(logFile,"%s|%hhu|%s|",buf,ID,ErrorLevel);

	va_start(ap, format);
	vfprintf(logFile, format, ap);
	va_end(ap);

	fprintf(logFile,"\n");
	fflush(logFile);

	pthread_mutex_unlock(&log_mutex);

    currentLogFileLines++;
    if (currentLogFileLines==MaxLogFileLines)
    {
        CloseLog();
        OpenLog();
    }


}

int WriteErrorNumber(const char *LogLevel)
{
	int result = -errno;

    WriteLog(LogLevel, "Returned error is (%i: %s)", errno, strerror(errno));

    return result;
}


void OpenAudit()
{

    if (WriteAuditFiles==0) return;

	if (FileExists("/var/log/WORM_Audit.log")==1) RenameAudit();

    currentAuditFileLines=0;
	auditFile = fopen("/var/log/WORM_Audit.log", "w");
	if (logFile == NULL)
	{
		perror("Failed to create audit file");
		exit(EXIT_FAILURE);
	}
 	setvbuf(auditFile, NULL, _IOLBF, 0);//*/

}

void RenameAudit()
{
	long long now;
	char fileName[PATH_MAX];

	now=time(NULL);
	sprintf(fileName,"/var/log/WORM_Audit-%llu.log",now);

	rename("/var/log/WORM_Audit.log",fileName);
}

void CloseAudit()
{
    if (WriteAuditFiles==0) return;

	fclose(auditFile);
    RenameAudit();

}
void WriteAudit(const char *Action,const char *Entity,const char* Result,const char* Path,const char *Value)
{
	long long now;
	struct fuse_context *context;

    if (WriteAuditFiles==0) return;

	pthread_mutex_lock(&audit_mutex);

	context=fuse_get_context();

	now=time(NULL);

	fprintf(auditFile,"%llu|%hhu|%s|%s|%s|%s|%s|%i|%i\n",now,ID,Action,Entity,Result,Path,Value,context->uid,context->gid);

	fflush(auditFile);


	pthread_mutex_unlock(&audit_mutex);

    currentAuditFileLines++;
    if (currentAuditFileLines==MaxAuditFileLines)
    {
        CloseAudit();
        OpenAudit();
    }

}
void AuditSuccess(const char *Action,const char *Entity,const char* Path,const char *Format, ...)
{
	va_list ap;
	char message[PATH_MAX];

    if (WriteAuditFiles==0) return;

	va_start(ap, Format);
	vsprintf(message,Format,ap);
	va_end(ap);

	WriteAudit(Action,Entity,SUCCESS,Path,message);
}
void AuditFailure(const char *Action,const char *Entity,const char* Path,const char *Format, ...)
{
	va_list ap;
	char message[PATH_MAX];

    if (WriteAuditFiles==0) return;

	va_start(ap, Format);
	vsprintf(message,Format,ap);
	va_end(ap);

	WriteAudit(Action,Entity,FAILURE,Path,message);
}

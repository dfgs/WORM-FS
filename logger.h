#ifndef _Logger_
#define _Logger_

#include <limits.h>
#include <log.h>
#include <mqueue.h>

//extern int MaxLinesInLogFiles;

extern const char* DEBUG;
extern const char* INFO;
extern const char* WARN;
extern const char* ERROR;

extern const char* SUCCESS;
extern const char* FAILURE;
extern const char* OK;
extern const char* NOK;
extern const char* NOTEXPIRED;

extern const char* CREATE;
extern const char* DELETE;
extern const char* UPDATE;

extern const char* cFILE;
extern const char* DIRECTORY;
extern const char* EXPIRATION;
extern const char* LOCK;
extern const char* RETENTION;
extern const char* OWNER;
extern const char* MODE;
extern const char* TIME;
extern const char* LOCATION;
extern const char* ATTRIBUTE;

typedef struct
{
	LogFile* logFile;
	LogFile* auditFile;
	mqd_t auditQueue;
} Logger;


extern void (*writeAudit)(const char*,const char*,const char* ,const char* ,const char*);


int logger_init(void);
void logger_free(void);

void logger_enter(const char *funcName,const char* path);
void logger_log(const char* funcName,const char* path,const char* errorLevel,const char *format, ...);
int logger_errno(const char* funcName,const char* path,const char* message);

//void writeAudit(const char *action,const char *entity,const char* result,const char* path,const char *value);
void logger_auditSuccess(const char *action,const char *entity,const char* path,const char *format, ...);
void logger_auditFailure(const char *action,const char *entity,const char* path,const char *format, ...);

#endif

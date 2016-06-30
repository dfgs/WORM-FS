#ifndef _Logger_
#define _Logger_

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

extern unsigned char ID;
extern int maxLogFileLines;
extern int maxAuditFileLines;
extern int writeAuditFiles;
extern int lockDelay;
extern int autoLock;

//void StartAuditTimer();
//void StopAuditTimer();
void initLog(void);
void disposeLog(void);
void renameLog(void);
void openLog(void);
void closeLog(void);

void logEnter(const char *funcName,const char* path);
void writeLog(const char* funcName,const char* path,const char *errorLevel,const char *format, ...);
int writeErrorNumber(const char* funcName,const char* path);

//void writeLogHeader();

void openAudit(void);
void closeAudit(void);
void renameAudit(void);
void writeAudit(const char *action,const char *entity,const char* result,const char* path,const char *value);
void auditSuccess(const char *action,const char *entity,const char* path,const char *format, ...);
void auditFailure(const char *action,const char *entity,const char* path,const char *format, ...);

#endif

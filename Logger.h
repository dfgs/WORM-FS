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
extern const char* RETENTION;
extern const char* OWNER;
extern const char* MODE;
extern const char* TIME;
extern const char* LOCATION;
extern const char* ATTRIBUTE;

extern unsigned char ID;
extern int MaxLogFileLines;
extern int MaxAuditFileLines;
extern int WriteAuditFiles;

//void StartAuditTimer();
//void StopAuditTimer();
void InitLog(void);
void DisposeLog(void);
void RenameLog(void);
void OpenLog(void);
void CloseLog(void);
void LogEnter(const char *Function);
void WriteLog(const char *LogLevel,const char *format, ...);
int WriteErrorNumber(const char *LogLevel);
//void WriteLogHeader();

void OpenAudit(void);
void CloseAudit(void);
void RenameAudit(void);
void WriteAudit(const char *Action,const char *Entity,const char* Result,const char* Path,const char *Value);
void AuditSuccess(const char *Action,const char *Entity,const char* Path,const char *Format, ...);
void AuditFailure(const char *Action,const char *Entity,const char* Path,const char *Format, ...);

#endif

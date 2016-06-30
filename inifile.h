#ifndef _INIFILE_
#define _INIFILE_

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair
{
   char* Key;
   char* Value;
};


typedef struct Section Section;
struct Section
{
    char* Name;
    int currentAllocation;
    int Count;
    KeyValuePair *Items;
};

typedef struct SectionDictionary SectionDictionary;
struct SectionDictionary
{
    int currentAllocation;
    int Count;
    Section *Items;
};

void free_ini(SectionDictionary* SectionDictionary);
void dump_ini(SectionDictionary* SectionDictionary);
SectionDictionary* open_ini(const char* FileName);
char* ini_getString(SectionDictionary *SectionDictionary, const char* Section,const char* Key,char* Default);
int ini_getInt(SectionDictionary *SectionDictionary, const char* Section,const char* Key,int Default);
Section* ini_getSection(SectionDictionary *SectionDictionary, const char* Name);
KeyValuePair* ini_getKeyValuePair(Section *Section, const char* Name);




#endif // _INIFILE_

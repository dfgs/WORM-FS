#ifndef _INIFILE_
#define _INIFILE_

typedef struct KeyValuePair KeyValuePair;
struct KeyValuePair
{
   char* key;
   char* value;
};


typedef struct Section Section;
struct Section
{
    char* name;
    int currentAllocation;
    int count;
    KeyValuePair *items;
};

typedef struct SectionDictionary SectionDictionary;
struct SectionDictionary
{
    int currentAllocation;
    int count;
    Section *items;
};

void free_ini(SectionDictionary* sectionDictionary);
void dump_ini(SectionDictionary* sectionDictionary);
SectionDictionary* open_ini(const char* fileName);
char* ini_getString(SectionDictionary *sectionDictionary, const char* section,const char* key,char* defaultValue);
int ini_getInt(SectionDictionary *sectionDictionary, const char* section,const char* key,int defaultValue);
unsigned short ini_getUnsignedShort(SectionDictionary *sectionDictionary, const char* sectionName,const char* keyName,unsigned short defaultValue);
Section* ini_getSection(SectionDictionary *sectionDictionary, const char* name);
KeyValuePair* ini_getKeyValuePair(Section *section, const char* name);




#endif // _INIFILE_

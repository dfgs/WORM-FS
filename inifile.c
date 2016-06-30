#include "inifile.h"
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#define MAXLINELENGTH 1000

static SectionDictionary* createDictionary()
{
    SectionDictionary* sectionDictionary;

    sectionDictionary=malloc(sizeof(SectionDictionary));
    sectionDictionary->count=0;
    sectionDictionary->currentAllocation=5;
    sectionDictionary->items=malloc(sizeof(Section)*sectionDictionary->currentAllocation);

    return sectionDictionary;
}

static Section* addSection(SectionDictionary* sectionDictionary)
{
    Section* section;
    Section* result;

    if (sectionDictionary->currentAllocation==sectionDictionary->count)
    {
        sectionDictionary->currentAllocation+=5;
        result=realloc(sectionDictionary->items,sizeof(Section)*sectionDictionary->currentAllocation);
        if (result==NULL) perror("Not enough memory");
        else sectionDictionary->items=result;
    }

    section=&sectionDictionary->items[sectionDictionary->count];
    section->count=0;
    section->currentAllocation=5;
    section->items=malloc(sizeof(KeyValuePair)*section->currentAllocation);
    sectionDictionary->count++;

    return section;
}

static KeyValuePair* addKeyValuePair(Section* section)
{
    KeyValuePair* keyValuePair;
    KeyValuePair* result;

    if (section->currentAllocation==section->count)
    {
        section->currentAllocation+=5;
        result=realloc(section->items,sizeof(KeyValuePair)*section->currentAllocation);
        if (result==NULL) perror("Not enough memory");
        else section->items=result;
    }

    keyValuePair=&section->items[section->count];
    section->count++;

    return keyValuePair;
}

void free_ini(SectionDictionary* sectionDictionary)
{
    int t;
    int s;
    Section section;
    KeyValuePair keyValuePair;

    for(t=0;t<sectionDictionary->count;t++)
    {
        section=sectionDictionary->items[t];
        free(section.name);
        for(s=0;s<section.count;s++)
        {
            keyValuePair=section.items[s];
            free(keyValuePair.key);
            free(keyValuePair.value);
        }
        free(section.items);
    }

    free(sectionDictionary->items);
    free(sectionDictionary);
}
void dump_ini(SectionDictionary* sectionDictionary)
{
    int t;
    int s;
    Section section;
    KeyValuePair keyValuePair;

    for(t=0;t<sectionDictionary->count;t++)
    {
        section=sectionDictionary->items[t];
        printf("[%s]\n",section.name);
        for(s=0;s<section.count;s++)
        {
            keyValuePair=section.items[s];
            printf("%s=%s\n",keyValuePair.key,keyValuePair.value);
         }
        printf("\n");
    }


}

SectionDictionary* open_ini(const char* fileName)
{
    FILE *file;
    char* line;
    regex_t commentRegex,emptyRegex,sectionRegex,keyRegex;
    regmatch_t *matches = NULL;
    int start,end,length;
    SectionDictionary* dictionary;
    Section* currentSection;
    KeyValuePair* keyValuePair;

    file = fopen(fileName, "r");
    if (file==NULL)
    {
        perror("Cannot open ini file");
        return NULL;
    }

    dictionary=createDictionary();

    regcomp(&emptyRegex,"^[ \t\n]*$",REG_EXTENDED | REG_NOSUB );
    regcomp(&commentRegex,"^[ \t\n]*#",REG_EXTENDED | REG_NOSUB);
    regcomp(&sectionRegex,"^[ \t\n]*\\[([^]#]+)\\][ \t\n]*(#.*)?$",REG_EXTENDED);
    regcomp(&keyRegex,"^[ \t\n]*([^= \t]+)[ \t\n]*=[ \t\n]*((\"[^\"]*\")|([^# \t\n]+))[ \t\n]*(#.*)?$",REG_EXTENDED);

    matches = malloc (sizeof (*matches) * 5);
    line=malloc(sizeof(*line)*MAXLINELENGTH);


    currentSection=addSection(dictionary);
    currentSection->name=malloc(sizeof(char)*8);
    strncpy (currentSection->name, "default", 7);
    currentSection->name[7]=0;

    while(fgets(line, MAXLINELENGTH, file)!=NULL)
    {
        if (regexec(&emptyRegex, line, 0, NULL, 0)==0) continue;    // empty line
        if (regexec(&commentRegex, line, 0, NULL, 0)==0) continue;    // comment line
        if (regexec(&sectionRegex, line, 5, matches, 0)==0)    // new section
        {
            start = matches[1].rm_so;
            end = matches[1].rm_eo;
            length = end - start;

            currentSection=addSection(dictionary);
            currentSection->name=malloc(sizeof(char)*length);
            strncpy (currentSection->name, line+start, length);
            currentSection->name[length]=0;

            //printf("New section: %s\n",currentSection.name);
            continue;
        }
        if (regexec(&keyRegex, line, 5, matches, 0)==0)    // new keyvalue pair
        {
            keyValuePair=addKeyValuePair(currentSection);

            start = matches[1].rm_so;
            end = matches[1].rm_eo;
            length = end - start;
            keyValuePair->key=malloc(length+1);
            strncpy (keyValuePair->key, line+start, length);
            keyValuePair->key[length]=0;


            start = matches[2].rm_so;
            end = matches[2].rm_eo;
            if (line[start]=='"') { start++;end--;}
            length = end - start;
            keyValuePair->value=malloc(length+1);
            if (length>0) strncpy (keyValuePair->value, line+start, length);
            keyValuePair->value[length]=0;

            //printf("New key value pair: %s / %s\n",keyValuePair.key,keyValuePair.value);
            continue;
        }

        fprintf(stderr,"Syntax error in ini file: %s",line);
        //fprintf(stderr,line);
    }

    fclose(file);

    free(matches);
    free(line);

    regfree(&keyRegex);
    regfree(&sectionRegex);
    regfree(&commentRegex);
    regfree(&emptyRegex);

    return dictionary;

}

Section* ini_getSection(SectionDictionary *sectionDictionary, const char* name)
{
    int t;
    Section* section;

    for(t=0;t<sectionDictionary->count;t++)
    {
        section=&sectionDictionary->items[t];
        if (strcmp(section->name,name)==0) return section;
    }

    return NULL;
}
KeyValuePair* ini_getKeyValuePair(Section *section, const char* name)
{
    int t;
    KeyValuePair* keyValuePair;

    for(t=0;t<section->count;t++)
    {
        keyValuePair=&section->items[t];
        if (strcmp(keyValuePair->key,name)==0) return keyValuePair;
    }

    return NULL;
}

char* ini_getString(SectionDictionary *sectionDictionary, const char* sectionName,const char* keyName,char* defaultValue)
{
    Section* section;
    KeyValuePair* keyValuePair;

    section=ini_getSection(sectionDictionary,sectionName);
    if (section==NULL) return defaultValue;
    keyValuePair=ini_getKeyValuePair(section,keyName);
    if (keyValuePair==NULL) return defaultValue;

    return keyValuePair->value;

}

int ini_getInt(SectionDictionary *sectionDictionary, const char* sectionName,const char* keyName,int defaultValue)
{
    Section* section;
    KeyValuePair* keyValuePair;
    int result;

    section=ini_getSection(sectionDictionary,sectionName);
    if (section==NULL) return defaultValue;
    keyValuePair=ini_getKeyValuePair(section,keyName);
    if (keyValuePair==NULL) return defaultValue;

    if (sscanf(keyValuePair->value, "%d", &result)==1) return result;
    else return defaultValue;

}
unsigned short ini_getUnsignedShort(SectionDictionary *sectionDictionary, const char* sectionName,const char* keyName,unsigned short defaultValue)
{
    Section* section;
    KeyValuePair* keyValuePair;
    unsigned short result;

    section=ini_getSection(sectionDictionary,sectionName);
    if (section==NULL) return defaultValue;
    keyValuePair=ini_getKeyValuePair(section,keyName);
    if (keyValuePair==NULL) return defaultValue;

    if (sscanf(keyValuePair->value, "%hu", &result)==1) return result;
    else return defaultValue;

}

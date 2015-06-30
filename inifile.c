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
    sectionDictionary->Count=0;
    sectionDictionary->currentAllocation=5;
    sectionDictionary->Items=malloc(sizeof(Section)*sectionDictionary->currentAllocation);

    return sectionDictionary;
}

static Section* addSection(SectionDictionary* SectionDictionary)
{
    Section* section;
    Section* result;

    if (SectionDictionary->currentAllocation==SectionDictionary->Count)
    {
        SectionDictionary->currentAllocation+=5;
        result=realloc(SectionDictionary->Items,sizeof(Section)*SectionDictionary->currentAllocation);
        if (result==NULL) perror("Not enough memory");
        else SectionDictionary->Items=result;
    }

    section=&SectionDictionary->Items[SectionDictionary->Count];
    section->Count=0;
    section->currentAllocation=5;
    section->Items=malloc(sizeof(KeyValuePair)*section->currentAllocation);
    SectionDictionary->Count++;

    return section;
}

static KeyValuePair* addKeyValuePair(Section* Section)
{
    KeyValuePair* keyValuePair;
    KeyValuePair* result;

    if (Section->currentAllocation==Section->Count)
    {
        Section->currentAllocation+=5;
        result=realloc(Section->Items,sizeof(KeyValuePair)*Section->currentAllocation);
        if (result==NULL) perror("Not enough memory");
        else Section->Items=result;
    }

    keyValuePair=&Section->Items[Section->Count];
    Section->Count++;

    return keyValuePair;
}

void free_ini(SectionDictionary* SectionDictionary)
{
    int t;
    int s;
    Section section;
    KeyValuePair keyValuePair;

    for(t=0;t<SectionDictionary->Count;t++)
    {
        section=SectionDictionary->Items[t];
        free(section.Name);
        for(s=0;s<section.Count;s++)
        {
            keyValuePair=section.Items[s];
            free(keyValuePair.Key);
            free(keyValuePair.Value);
        }
        free(section.Items);
    }

    free(SectionDictionary->Items);
    free(SectionDictionary);
}
void dump_ini(SectionDictionary* SectionDictionary)
{
    int t;
    int s;
    Section section;
    KeyValuePair keyValuePair;

    for(t=0;t<SectionDictionary->Count;t++)
    {
        section=SectionDictionary->Items[t];
        printf("[%s]\n",section.Name);
        for(s=0;s<section.Count;s++)
        {
            keyValuePair=section.Items[s];
            printf("%s=%s\n",keyValuePair.Key,keyValuePair.Value);
         }
        printf("\n");
    }


}

SectionDictionary* open_ini(const char* FileName)
{
    FILE *file;
    char* line;
    regex_t commentRegex,emptyRegex,sectionRegex,keyRegex;
    regmatch_t *matches = NULL;
    int start,end,length;
    SectionDictionary* dictionary;
    Section* currentSection;
    KeyValuePair* keyValuePair;

    file = fopen(FileName, "r");
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
    currentSection->Name=malloc(sizeof(char)*8);
    strncpy (currentSection->Name, "default", 7);
    currentSection->Name[7]=0;

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
            currentSection->Name=malloc(sizeof(char)*length);
            strncpy (currentSection->Name, line+start, length);
            currentSection->Name[length]=0;

            //printf("New section: %s\n",currentSection.Name);
            continue;
        }
        if (regexec(&keyRegex, line, 5, matches, 0)==0)    // new keyvalue pair
        {
            keyValuePair=addKeyValuePair(currentSection);

            start = matches[1].rm_so;
            end = matches[1].rm_eo;
            length = end - start;
            keyValuePair->Key=malloc(length+1);
            strncpy (keyValuePair->Key, line+start, length);
            keyValuePair->Key[length]=0;


            start = matches[2].rm_so;
            end = matches[2].rm_eo;
            if (line[start]=='"') { start++;end--;}
            length = end - start;
            keyValuePair->Value=malloc(length+1);
            if (length>0) strncpy (keyValuePair->Value, line+start, length);
            keyValuePair->Value[length]=0;

            //printf("New key value pair: %s / %s\n",keyValuePair.Key,keyValuePair.Value);
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

Section* ini_getSection(SectionDictionary *SectionDictionary, const char* Name)
{
    int t;
    Section* section;

    for(t=0;t<SectionDictionary->Count;t++)
    {
        section=&SectionDictionary->Items[t];
        if (strcmp(section->Name,Name)==0) return section;
    }

    return NULL;
}
KeyValuePair* ini_getKeyValuePair(Section *Section, const char* Name)
{
    int t;
    KeyValuePair* keyValuePair;

    for(t=0;t<Section->Count;t++)
    {
        keyValuePair=&Section->Items[t];
        if (strcmp(keyValuePair->Key,Name)==0) return keyValuePair;
    }

    return NULL;
}

char* ini_getString(SectionDictionary *SectionDictionary, const char* SectionName,const char* KeyName,char* Default)
{
    Section* section;
    KeyValuePair* keyValuePair;

    section=ini_getSection(SectionDictionary,SectionName);
    if (section==NULL) return Default;
    keyValuePair=ini_getKeyValuePair(section,KeyName);
    if (keyValuePair==NULL) return Default;

    return keyValuePair->Value;

}

int ini_getInt(SectionDictionary *SectionDictionary, const char* SectionName,const char* KeyName,int Default)
{
    Section* section;
    KeyValuePair* keyValuePair;
    int result;

    section=ini_getSection(SectionDictionary,SectionName);
    if (section==NULL) return Default;
    keyValuePair=ini_getKeyValuePair(section,KeyName);
    if (keyValuePair==NULL) return Default;

    if (sscanf(keyValuePair->Value, "%d", &result)==1) return result;
    else return Default;

}
unsigned short ini_getUnsignedShort(SectionDictionary *SectionDictionary, const char* SectionName,const char* KeyName,unsigned short Default)
{
    Section* section;
    KeyValuePair* keyValuePair;
    unsigned short result;

    section=ini_getSection(SectionDictionary,SectionName);
    if (section==NULL) return Default;
    keyValuePair=ini_getKeyValuePair(section,KeyName);
    if (keyValuePair==NULL) return Default;

    if (sscanf(keyValuePair->Value, "%hu", &result)==1) return result;
    else return Default;

}

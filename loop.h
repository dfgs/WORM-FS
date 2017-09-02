#ifndef _Loop_
#define _Loop_

typedef struct 
{
    char name[1024];
    const char* fileName;
	const char* mountPoint;
} Loop;

Loop* loop_create(const char* fileName,const char* mountPoint);
int loop_mount(Loop* loop);
int loop_umount(Loop* loop);
void loop_free(Loop* loop);


#endif 


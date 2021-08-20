#include "ls.h"

typedef struct 
{
    int fileSize;
    char fileName[NAME_MAX];
} entry_t;

typedef struct
{
    int orderBySize;

} options;


static int countDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader);
static void populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *absDir);
static void printDirectoryEntries(entry_t entries[], int numEntries);
static char *formatDirectoryParmeters(int argc, char **argv);
static int qsortCompareName(const void *a, const void *b);
static int qsortCompareSize(const void *a, const void *b);
static void sortEntries(entry_t entries[], int numEntries);

int main(int argc, char **argv)
{
    DIR *directoryPointer;
    struct dirent *directoryReader;
    int numEntries;
    char *option;
    int o;

    while((o = getopt(argc, argv, option)) != -1){

    } 

    char *absDir = formatDirectoryParmeters(argc, argv);
    directoryPointer = opendir(absDir);
    numEntries = countDirectoryEntries(directoryPointer, directoryReader);
    
    entry_t entries[numEntries];
    rewinddir(directoryPointer); 
    populateDirectoryEntries(directoryPointer, directoryReader, entries, absDir);
    sortEntries(entries, numEntries);
    printDirectoryEntries(entries, numEntries);
    closedir(directoryPointer);
    return 0;
}

static char *formatDirectoryParmeters(int argc, char **argv)
{
    if(argc > 1){
        char *dirName = argv[1];
        while(*++dirName != '\0');   
        if(*--dirName != '/')
            return strcat(argv[1], "/");
        return argv[1];
    } else {
        return "./";
    }
}

static int countDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader)
{
    int count = 0;
    while((directoryReader = readdir(directoryPointer)) != NULL){
        count++;
    }
    return count;
}

static void populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *absDir)
{
    struct stat stat_buf;
    char full_name[NAME_MAX];
    
    while((directoryReader = readdir(directoryPointer)) != NULL){
        strcpy(entries->fileName, directoryReader->d_name);
        sprintf(full_name, "%s%s", absDir, entries->fileName);
        stat(full_name, &stat_buf);
        entries->fileSize = stat_buf.st_size;
        entries++;
    }
}

static void sortEntries(entry_t entries[], int numEntries)
{
    qsort(entries, numEntries, sizeof(entries[0]), qsortCompareSize);
}

static void printDirectoryEntries(entry_t entries[], int numEntries)
{
    for(int i = 0; i < numEntries; i++){
        printf("%10d %s \n", entries[i].fileSize, entries[i].fileName);
    }
}

static int qsortCompareName(const void *a, const void *b)
{
    entry_t *entryA = (entry_t *)a;
    entry_t *entryB = (entry_t *)b;
    return strcmp(entryA->fileName, entryB->fileName);
}

static int qsortCompareSize(const void *a, const void *b)
{
    entry_t *entryA = (entry_t *)a;
    entry_t *entryB = (entry_t *)b;
    return (entryA->fileSize - entryB->fileSize);
}
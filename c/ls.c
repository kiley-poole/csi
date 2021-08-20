#include "ls.h"

typedef struct 
{
    int fileSize;
    char fileName[NAME_MAX];
} entry_t;

typedef struct
{
    int orderBySize;

} option_param_t;

static void processLoop(char *dirName);
static int countDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader);
static void populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *path);
static void printDirectoryEntries(entry_t entries[], int numEntries);
static char *formatDirectoryParmeters(char *dirName);
static int qsortCompareName(const void *a, const void *b);
static int qsortCompareSize(const void *a, const void *b);
static void sortEntries(entry_t entries[], int numEntries);
static void toggleOptions(int argc, char **argv);

static option_param_t options;
static int numOfDirectoryParms;

int main(int argc, char **argv)
{
    options.orderBySize = 0;
    toggleOptions(argc, argv);
    numOfDirectoryParms = argc - optind;
    do
    {
        processLoop(argv[optind]);
        printf("\n\n");
        numOfDirectoryParms = argc - ++optind;
    } while (numOfDirectoryParms > 0);
    
    return 0;
}

static void processLoop(char *dirName)
{
    DIR *directoryPointer;
    struct dirent *directoryReader;
    
    char *path = formatDirectoryParmeters(dirName);
    directoryPointer = opendir(path);
    int numEntries = countDirectoryEntries(directoryPointer, directoryReader);
    
    entry_t entries[numEntries];
    rewinddir(directoryPointer); 
    populateDirectoryEntries(directoryPointer, directoryReader, entries, path);
    sortEntries(entries, numEntries);
    printDirectoryEntries(entries, numEntries);
    closedir(directoryPointer);
}

static void toggleOptions(int argc, char **argv)
{
    int c;

    while((c = getopt(argc, argv, "S")) != -1){
        switch (c)
        {
        case 'S':
            options.orderBySize = 1;
            break;
        
        default:
            fprintf(stderr, "Unrecognized option parameter(s). Supported parameters: S. \n");
            break;
        }

    } 
}

static char *formatDirectoryParmeters(char *dirName)
{
    if(numOfDirectoryParms > 0){ 
        if(dirName[strlen(dirName)-1] != '/')
            return strcat(dirName, "/");
        return dirName;
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

static void populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *path)
{
    struct stat stat_buf;
    char full_name[NAME_MAX];
    
    while((directoryReader = readdir(directoryPointer)) != NULL){
        strcpy(entries->fileName, directoryReader->d_name);
        sprintf(full_name, "%s%s", path, entries->fileName);
        stat(full_name, &stat_buf);
        entries->fileSize = stat_buf.st_size;
        entries++;
    }
}

static void sortEntries(entry_t entries[], int numEntries)
{
    if(options.orderBySize){
        qsort(entries, numEntries, sizeof(entries[0]), qsortCompareSize);
    } else {
        qsort(entries, numEntries, sizeof(entries[0]), qsortCompareName);
    }
    
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
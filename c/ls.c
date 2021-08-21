#include "ls.h"

typedef struct 
{
    char   fileName[NAME_MAX];
    off_t  fileSize;
    mode_t mode;    
    gid_t  gid;
    uid_t  uid;

} entry_t;

typedef struct
{
    int orderBySize;
    int showAll;
    int longFormat;

} option_param_t;

static void toggleOptions(int argc, char **argv);
static void processLoop(char *path);
static void formatDirectoryParmeter(char *path, char *formattedPath);
static int  countDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader);
static void populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *path);
static void printDirectoryEntries(entry_t entries[], int numEntries);
static int  qsortCompareName(const void *a, const void *b);
static int  qsortCompareSize(const void *a, const void *b);
static void sortEntries(entry_t entries[], int numEntries);
static char *formatColor(mode_t entryMode);
static char *getUserName(uid_t uid);
static char *getGroupName(gid_t gid);

static option_param_t options;
static int numOfDirectoryParms;

int main(int argc, char **argv)
{

    options.orderBySize = 0;
    options.showAll = 0;
    options.longFormat = 0;
    toggleOptions(argc, argv);

    numOfDirectoryParms = argc - optind;
    do
    {   
        processLoop(argv[optind]);
        if(numOfDirectoryParms > 1)
            printf("\n");
        numOfDirectoryParms = argc - ++optind;
    } while (numOfDirectoryParms > 0);
    
    return 0;
}

static void toggleOptions(int argc, char **argv)
{
    int c;

    while((c = getopt(argc, argv, "Sal")) != -1){
        switch (c)
        {
        case 'S':
            options.orderBySize = 1;
            break;
        case 'a':
            options.showAll = 1;
            break;
        case 'l':
            options.longFormat = 1;
            break;
        default:
            fprintf(stderr, "Unrecognized option parameter(s). Supported parameters: S - Sort by Size, a - show all, l - long format. \n");
            break;
        }

    } 
}

static void processLoop(char *path)
{
    DIR *directoryPointer;
    struct dirent *directoryReader;
    char formattedPath[NAME_MAX];
    
    formatDirectoryParmeter(path, formattedPath);
    directoryPointer = opendir(formattedPath);
    int numEntries = countDirectoryEntries(directoryPointer, directoryReader);
    
    entry_t entries[numEntries];

    rewinddir(directoryPointer); 
    populateDirectoryEntries(directoryPointer, directoryReader, entries, formattedPath);
    sortEntries(entries, numEntries);
    printf("%s\n", formattedPath);
    printDirectoryEntries(entries, numEntries);
    closedir(directoryPointer);
}

static void formatDirectoryParmeter(char *path, char *formattedPath)
{
    if(numOfDirectoryParms > 0){
        strcpy(formattedPath, path); 
        if(path[strlen(path)-1] != '/')
            strcat(formattedPath, "/");
    } else {
        strcpy(formattedPath, "./"); 
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
        entries->mode = stat_buf.st_mode;
        entries->gid = stat_buf.st_gid;
        entries->uid = stat_buf.st_uid;
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
    char *color, *userName, *groupName;    

    for(int i = 0; i < numEntries; i++){
        printf("%s", WHITE);

        if(options.longFormat){
            userName = getUserName(entries[i].uid);
            groupName = getGroupName(entries[i].gid);
            printf("%s %s %10d", userName, groupName, (int) entries[i].fileSize);
        }

        if(*entries[i].fileName != '.' || options.showAll){
            color = formatColor(entries[i].mode);
            printf("%s %s \n", color, entries[i].fileName);
        }
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

static char *formatColor(mode_t entryMode)
{
    if(S_ISDIR(entryMode)){
        return BLUE;
    }
    if(S_ISLNK(entryMode)){
        return CYAN;
    }
    if(entryMode & S_IXUSR){
        return GREEN;
    }
    if(S_ISREG(entryMode)){
        return WHITE;
    }
}

static char *getUserName(uid_t uid)
{
    struct passwd *user;
    user = getpwuid(uid);
    return user->pw_name;
}

static char *getGroupName(gid_t gid)
{
    struct group *group;
    group = getgrgid(gid);
    return group->gr_name;

}
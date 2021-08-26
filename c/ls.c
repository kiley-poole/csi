#include "ls.h"

typedef struct 
{
    char   fileName[NAME_MAX];
    off_t  fileSize;
    mode_t mode;    
    gid_t  gid;
    uid_t  uid;
    blkcnt_t blocks;
    time_t modTime;

} entry_t;

typedef struct
{
    int orderBySize;
    int showAll;
    int longFormat;
    int blockCount;

} option_param_t;

static void toggleOptions(int argc, char **argv);
static void processLoop(char *path);
static void formatDirectoryParmeter(char *path, char *formattedPath);
static int  countDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader);
static int  populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *path);
static void printDirectoryEntries(entry_t entries[], int numEntries);
static int  qsortCompareName(const void *a, const void *b);
static int  qsortCompareSize(const void *a, const void *b);
static void sortEntries(entry_t entries[], int numEntries);
static char *formatColor(mode_t entryMode);
static char *getUserName(uid_t uid);
static char *getGroupName(gid_t gid);
static char *getDateTime(time_t modTime);
static void printPermissions(mode_t entryMode);

static option_param_t options;
static int numOfDirectoryParms;
static int userPad;
static int groupPad;

int main(int argc, char **argv)
{

    options.orderBySize = 0;
    options.showAll = 0;
    options.longFormat = 0;
    options.blockCount = 0;
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

    while((c = getopt(argc, argv, "Sals")) != -1){
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
        case 's':
            options.blockCount = 1;
            break;
        default:
            fprintf(stderr, "Unrecognized option parameter(s). Supported parameters: S - Sort by Size, a - show all, l - long format, s - block count \n");
            break;
        }

    } 
}

static void processLoop(char *path)
{
    DIR *directoryPointer;
    struct dirent *directoryReader;
    char formattedPath[NAME_MAX];
    int totalBlocks;
    
    formatDirectoryParmeter(path, formattedPath);
    directoryPointer = opendir(formattedPath);
    int numEntries = countDirectoryEntries(directoryPointer, directoryReader);
    
    entry_t entries[numEntries];

    rewinddir(directoryPointer); 
    totalBlocks = populateDirectoryEntries(directoryPointer, directoryReader, entries, formattedPath);
    sortEntries(entries, numEntries);
    printf("%s%s:\n", WHITE, formattedPath);
    printf("total %d\n", totalBlocks);
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

static int populateDirectoryEntries(DIR *directoryPointer, struct dirent *directoryReader, entry_t entries[], char *path)
{
    struct stat stat_buf;
    char full_name[NAME_MAX];
    int totalBlocks;
    
    while((directoryReader = readdir(directoryPointer)) != NULL){
        strcpy(entries->fileName, directoryReader->d_name);
        sprintf(full_name, "%s%s", path, entries->fileName);
        stat(full_name, &stat_buf);
        entries->fileSize = stat_buf.st_size;
        entries->mode = stat_buf.st_mode;
        entries->gid = stat_buf.st_gid;
        entries->uid = stat_buf.st_uid;
        entries->modTime = stat_buf.st_mtime;
        entries->blocks = stat_buf.st_blocks / 2;
        totalBlocks += stat_buf.st_blocks / 2;
        entries++;
    }

    return totalBlocks;
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
    char *color, *userName, *groupName, *modTimeStamp;    

    for(int i = 0; i < numEntries; i++){
        printf("%s", WHITE);

        if(options.blockCount){
            printf("%2ld ", entries[i].blocks);
        }

        if(options.longFormat){
            userName = getUserName(entries[i].uid);
            groupName = getGroupName(entries[i].gid);
            modTimeStamp = getDateTime(entries[i].modTime);
            printPermissions(entries[i].mode);
            printf(" %10s %10s %10d %s", userName, groupName, (int) entries[i].fileSize, modTimeStamp);
        }

        if(*entries[i].fileName != '.' || options.showAll){
            color = formatColor(entries[i].mode);
            printf("%s %-100s \n", color, entries[i].fileName);
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

static char *getDateTime(time_t modTime)
{
    char *stringTime;
    strftime(stringTime, 100, "%b %y %H:%M", localtime(&modTime));
    return stringTime;
}

//https://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
static void printPermissions(mode_t entryMode)
{
    printf( (S_ISDIR(entryMode)) ? "d" : "-");
    printf( (entryMode & S_IRUSR) ? "r" : "-");
    printf( (entryMode & S_IWUSR) ? "w" : "-");
    printf( (entryMode & S_IXUSR) ? "x" : "-");
    printf( (entryMode & S_IRGRP) ? "r" : "-");
    printf( (entryMode & S_IWGRP) ? "w" : "-");
    printf( (entryMode & S_IXGRP) ? "x" : "-");
    printf( (entryMode & S_IROTH) ? "r" : "-");
    printf( (entryMode & S_IWOTH) ? "w" : "-");
    printf( (entryMode & S_IXOTH) ? "x" : "-");
}

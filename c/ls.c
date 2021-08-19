#include "ls.h"

typedef struct 
{
    int file_size;
    char file_name[NAME_MAX];
} entry_t;

static int count_directory_entries(DIR *directory_pointer, struct dirent *directory_reader);
static void populate_directory_entries(DIR *directory_pointer, struct dirent *directory_reader, entry_t entries[], char *abs_dir);
static void print_directory_entries(entry_t entries[], int num_entries);
static char *format_directory_parmeters(int argc, char **argv);
static int qsort_compare_name(const void *a, const void *b);
static void sort_entries(entry_t entries[], int num_entries);

int main(int argc, char **argv)
{
    DIR *directory_pointer;
    struct dirent *directory_reader;
    int num_entries;

    char *abs_dir = format_directory_parmeters(argc, argv);
    directory_pointer = opendir(abs_dir);
    num_entries = count_directory_entries(directory_pointer, directory_reader);
    
    entry_t entries[num_entries];
    rewinddir(directory_pointer); 
    populate_directory_entries(directory_pointer, directory_reader, entries, abs_dir);
    sort_entries(entries, num_entries);
    print_directory_entries(entries, num_entries);
    closedir(directory_pointer);
    return 0;
}

static char *format_directory_parmeters(int argc, char **argv)
{
    if(argc > 1){
        char *dir_name = argv[1];
        while(*++dir_name != '\0');   
        if(*--dir_name != '/')
            return strcat(argv[1], "/");
        return argv[1];
    } else {
        return "./";
    }
}

static int count_directory_entries(DIR *directory_pointer, struct dirent *directory_reader)
{
    int count = 0;
    while((directory_reader = readdir(directory_pointer)) != NULL){
        count++;
    }
    return count;
}

static void populate_directory_entries(DIR *directory_pointer, struct dirent *directory_reader, entry_t entries[], char *abs_dir)
{
    struct stat stat_buf;
    char full_name[NAME_MAX];
    while((directory_reader = readdir(directory_pointer)) != NULL){
        strcpy(entries->file_name, directory_reader->d_name);
        sprintf(full_name, "%s%s", abs_dir, entries->file_name);
        stat(full_name, &stat_buf);
        entries->file_size = stat_buf.st_size;
        entries++;
    }
}

static void sort_entries(entry_t entries[], int num_entries)
{
    qsort(entries, num_entries, sizeof(entries[0]), qsort_compare_name);
}

static void print_directory_entries(entry_t entries[], int num_entries)
{
    for(int i = 0; i < num_entries; i++){
        printf("%10d %s \n", entries[i].file_size, entries[i].file_name);
    }
}


static int qsort_compare_name(const void *a, const void *b)
{
    entry_t *entryA = (entry_t *)a;
    entry_t *entryB = (entry_t *)b;
    return strcmp(entryA->file_name, entryB->file_name);
}
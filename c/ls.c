#include "ls.h"

struct entry
{
    int file_size;
    char file_name[NAME_MAX];
};

static int count_directory_entries(DIR *directory_pointer, struct dirent *directory_reader);
static void populate_directory_entries(DIR *directory_pointer, struct dirent *directory_reader, struct entry entries[], char *abs_dir);
static void print_directory_entries(struct entry entries[], int num_entries);
static char *format_directory_parm(int argc, char **argv);


int main(int argc, char **argv)
{
    DIR *directory_pointer;
    struct dirent *directory_reader;
    int num_entries;
    char *abs_dir = format_directory_parm(argc, argv);

    directory_pointer = opendir(abs_dir);
    num_entries = count_directory_entries(directory_pointer, directory_reader);

    struct entry entries[num_entries];

    rewinddir(directory_pointer); 
    populate_directory_entries(directory_pointer, directory_reader, entries, abs_dir);
    print_directory_entries(entries, num_entries);
    closedir(directory_pointer);

    return 0;
}

static char *format_directory_parm(int argc, char **argv)
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

static void populate_directory_entries(DIR *directory_pointer, struct dirent *directory_reader, struct entry entries[], char *abs_dir)
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

static void print_directory_entries(struct entry entries[], int num_entries)
{
    for(int i = 0; i < num_entries; i++){
        printf("%s - %d \n", entries[i].file_name, entries[i].file_size);
    }
}
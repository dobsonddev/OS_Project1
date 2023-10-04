#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "process.h"


void read_proc_dir(Process **process_list, int *process_count){
    DIR *dir;
    struct dirent *entry;
    struct stat buf;
    char path[1024];
    int i = 0;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Unable to open /proc directory");
        exit(EXIT_FAILURE);
    }

    *process_count = 0;
    // Count the number of process directories first
    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "/proc/%s", entry->d_name);
        if(stat(path, &buf) == 0 && S_ISDIR(buf.st_mode) && isdigit(entry->d_name[0])) {
            (*process_count)++;
        }
    }
    rewinddir(dir);  // Reset directory stream to read again

    // Allocate memory for process_list based on the size of process_count
    *process_list = malloc((*process_count) * sizeof(Process));
    if (*process_list == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    // Populate process_list
    while ((entry = readdir(dir)) != NULL) {
        snprintf(path, sizeof(path), "/proc/%s", entry->d_name);
        if (stat(path, &buf) == 0 && S_ISDIR(buf.st_mode) && isdigit(entry->d_name[0])) {
            Process *proc = &(*process_list)[i];
            proc->pid = 0;
            proc->ppid = 0;
            proc->vsize = 0;
            proc->level = 0;
            proc->child_count = 0;
            proc->children = NULL;
            proc->pid = atoi(entry->d_name);
            read_stat_file(proc);
            i++;
        }
    }
    closedir(dir);
}


/*void read_stat_file(Process *proc) {
    char path[40];
    FILE *fp;

    sprintf(path, "/proc/%d/stat", proc->pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Unable to open stat file");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d %s %*c %d", &proc->pid, proc->comm, &proc->ppid);
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* buffer = (char*)malloc(filesize + 1);
    fread(buffer, filesize, 1, fp);
    buffer[filesize] = '\0';

    char* token = strrchr(buffer, ' ');
    if (token != NULL){
        proc->vsize = atoi(token);
    }

    free(buffer);
    fclose(fp);
}*/

void read_stat_file(Process *proc){
    char path[40];
    FILE *fp;

    sprintf(path, "/proc/%d/stat", proc->pid);
    fp = fopen(path, "r");
    if (fp == NULL){
        perror("Unable to open stat file");
        exit(EXIT_FAILURE);
    }

    //grab 22nd field so we can read 
    for(int i = 0; i < 22; i++){
        fscanf(fp, "%*s");
    }

    // grab v size at 23rd
    fscanf(fp, "%ld", &proc->vsize);

    //rewind the file
    rewind(fp);

    //standard grab
    fscanf(fp, "%d %s %*c %d", &proc->pid, proc->comm, &proc->ppid);

    memmove(&proc->comm[0], &proc->comm[1], strlen(proc->comm) -2);
    proc->comm[strlen(proc->comm) - 2] = '\0';

    fclose(fp);
}


void build_tree(Process **process_list, int process_count, Process ***roots, int *root_count){
    *root_count = 0;
    *roots = NULL;

    for (int i = 0; i < process_count; i++){
        if ((*process_list)[i].ppid == 0){
            *root_count += 1;
            *roots = realloc(*roots, *root_count * sizeof(Process *));
            (*roots)[*root_count - 1] = &(*process_list)[i];
            (*process_list)[i].level = 0;
        }
    }

    for (int i = 0; i < process_count; i++){
        for (int j = 0; j < process_count; j++){
            if((*process_list)[i].pid == (*process_list)[j].ppid){
                add_child(&(*process_list)[i], &(*process_list)[j]);
                (*process_list)[j].level = (*process_list)[i].level + 1;
            }
        }
    }
}


void display_tree(Process *root, int level){
    for (int i = 0; i < level; i++){
        printf("  ");
    }
    printf("(%d) %s, %ld kb\n", root->pid, root->comm, root->vsize / 1024);
    for(int i = 0; i < root->child_count; i++){
        display_tree(root->children[i], level + 1);
    }
}

void display_trees(Process **roots, int root_count) {
    for (int i = 0; i < root_count; i++){
        display_tree(roots[i], roots[i]->level);
    }
}

void add_child(Process *parent, Process *child){
    parent->children = realloc(parent->children, (parent->child_count + 1) * sizeof(Process *));
    if (parent->children == NULL){
        perror("mem alloc failed");
        exit(EXIT_FAILURE);
    }
    parent->children[parent->child_count] = child;
    child->parent = parent;
    parent->child_count++;
}

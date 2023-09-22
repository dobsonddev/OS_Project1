#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "process.h"

void read_proc_dir(Process **process_list, int *process_count) {
    DIR *dir;
    struct dirent *entry;
    int i = 0;

    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Unable to open /proc directory");
        exit(EXIT_FAILURE);
    }

    *process_count = 0;
    // Count the number of process directories first
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
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
        if (entry->d_type == DT_DIR && isdigit(entry->d_name[0])) {
            Process *proc = &(*process_list)[i];
            proc->pid = atoi(entry->d_name);
            read_stat_file(proc);  // Populate the fields in our "Process" struct, see process.h file for details
            i++;
        }
    }
    closedir(dir);
}



void read_stat_file(Process *proc) {
    char path[40];
    FILE *fp;

    sprintf(path, "/proc/%d/stat", proc->pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("Unable to open stat file");
        exit(EXIT_FAILURE);
    }

    fscanf(fp, "%d %s %*c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %u",
           &proc->pid, proc->comm, &proc->ppid, &proc->vsize);
    fclose(fp);
}


void build_tree(Process **process_list, int process_count, Process **root) {
    // Find the root (process with PID 1)
    for (int i = 0; i < process_count; i++) {
        if ((*process_list)[i].pid == 1) {
            *root = &(*process_list)[i];
            break;
        }
    }

    // TODO: Need to build out the parent-child relationships, AKA NEsting
}


void display_tree(Process *root, int level) {
    // Implement display of the tree here
}

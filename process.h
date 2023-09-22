#ifndef PROCESS_H
#define PROCESS_H

typedef struct Process {
    int pid;
    int ppid;
    int vsize;
    char comm[256];
    struct Process *parent;
    struct Process *children;
} Process;

void read_proc_dir(Process **process_list, int *process_count);
void read_stat_file(Process *proc);
void build_tree(Process **process_list, int process_count, Process **root);
void display_tree(Process *root, int level);

#endif //PROCESS_H

#include <stdio.h>
#include "process.h"

int main() {
    Process *process_list = NULL;
    int process_count = 0;
    Process **roots = NULL;
    int root_count = 0;

    // Read the /proc directory
    read_proc_dir(&process_list, &process_count);

    // Build the process tree
    build_tree(&process_list, process_count, &roots, &root_count);

    // Display the process tree
    display_trees(roots, root_count);

    return 0;
}
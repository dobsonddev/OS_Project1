#include <stdio.h>
#include "process.h"

int main() {
    Process *process_list = NULL;
    int process_count = 0;
    Process *root = NULL;

    // Read the /proc directory
    read_proc_dir(&process_list, &process_count);

    // Build the process tree
    build_tree(&process_list, process_count, &root);

    // Display the process tree
    display_tree(root, 0);

    return 0;
}

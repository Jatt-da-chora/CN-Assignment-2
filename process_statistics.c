#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

struct process_info {
    int pid;
    char name[256];
    unsigned long utime;
    unsigned long stime;
    unsigned long total_time;
};

int compare_process(const void *a, const void *b) {
    struct process_info *proc_a = (struct process_info *)a;
    struct process_info *proc_b = (struct process_info *)b;
    
    if (proc_b->total_time > proc_a->total_time)
        return 1;
    else if (proc_b->total_time < proc_a->total_time)
        return -1;
    else
        return 0;
}

int get_process_info(int pid, struct process_info *proc) {
    char path[64];
    FILE* fd;
    
    sprintf(path, "/proc/%d/stat", pid);

    fd = fopen(path, "r");
    if (fd == NULL) {
        return 0; 
    }

    unsigned long utime, stime;
    int read_pid;
    
    fscanf(fd, "%d %s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %lu %lu",
           &read_pid, &(proc -> name), &utime, &stime);

    proc->total_time = utime + stime;
    proc->pid = read_pid;
    proc->utime = utime;
    proc->stime = stime;

    fclose(fd);
    return 1;  
}


char* getTop() {
    struct process_info procs[1024];
    int count = 0;
    char *result = malloc(1024); 


    for (int pid = 1; pid < 32768; pid++) {  
        struct process_info proc;
        if (get_process_info(pid, &proc)) {
            procs[count++] = proc;  
        }
    }

    qsort(procs, count, sizeof(struct process_info), compare_process);

    if (count >= 2) {
        sprintf(result, "Top 2 CPU-consuming processes:\n"
                     "1. PID: %d, Process Name: %s, CPU Time: %lu clock ticks.\n"
                     "2. PID: %d, Process Name: %s, CPU Time: %lu clock ticks.\n",
                procs[0].pid, procs[0].name, procs[0].total_time,
                procs[1].pid, procs[1].name, procs[1].total_time);
    } else if (count == 1) {
        sprintf(result, "Top 1 CPU-consuming process:\n"
                     "1. PID: %d, Process Name: %s, CPU Time: %lu clock ticks.\n",
                procs[0].pid, procs[0].name, procs[0].total_time);
    } else {
        strcpy(result, "No processes found.");
    }
    
    return result;  
}

// main.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Cgroup;

extern struct Cgroup* create_cgroup(const char* group_name);
extern void limit_cpu_usage(struct Cgroup* cgroup, long max_cpu);
extern void limit_mem_usage(struct Cgroup* cgroup, long max_memory);
extern void add_pid(struct Cgroup* cgroup, unsigned long pid);
extern void free_cgroup(struct Cgroup* cgroup);

void intensive_task() {
  size_t size = 0;
  while (1) {
    int * data =  malloc(1024*1024);  // Allocate 1KB
    if (data== NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      break;
    }
    for(int i =0;i<1024;i++)
    for(int j =0;j<1000000;j++)
    data[i]+=data[i+1];
    size += 1;
    printf("Allocated memory: %lu kb \n", size);
  }
}

int main() {
  struct Cgroup* cgroup = create_cgroup("my_cgroup");
  printf("Adding current process:%d to cgroup\n", getpid());
  //add_pid(cgroup, getpid());

  // Set CPU limit to 10%

  //limit_cpu_usage(cgroup, 10);
  printf("Set CPU limit to 10%%\n");

  printf("Running CPU intensive task");
  // Set memory limit to 10KB
  limit_mem_usage(cgroup, 10 * 1024);
  //printf("Set memory limit to 10KB\n");

  // Run memory intensive task
  //printf("Running memory intensive task...\n");
  intensive_task();

  // Free the cgroup cgroup
  //free_cgroup(cgroup);

  return 0;
}

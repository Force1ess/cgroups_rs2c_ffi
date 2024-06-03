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

void mem_intensive_task() {
  size_t size = 0;
  while (1) {
    void* mem = malloc(1024);  // Allocate 1KB
    if (mem == NULL) {
      fprintf(stderr, "Memory allocation failed\n");
      break;
    }
    size += 1024;
    memset(mem, 0, 1024);  // Use the memory
    printf("Allocated memory: %lu bytes\n", size);
    sleep(1);  // Sleep for a second before next allocation
  }
}

int main() {
  struct Cgroup* manager = create_cgroup("my_cgroup");
  if (manager == NULL) {
    fprintf(stderr, "Failed to create cgroup manager\n");
    return 1;
  }

  // Add current process to the cgroup
  printf("Adding current process:%d to cgroup\n", getpid());
  add_pid(manager, getpid());

  // Set CPU limit to 10%
  limit_cpu_usage(manager, 10);
  printf("Set CPU limit to 10%%\n");

  printf("Running CPU intensive task");
  for (volatile unsigned long i = 0; i < 1000000000UL; i++) {
    // Busy loopxx
  }

  // Set memory limit to 10KB
  limit_mem_usage(manager, 10 * 1024);
  printf("Set memory limit to 10KB\n");

  // Run memory intensive task
  printf("Running memory intensive task...\n");
  mem_intensive_task();

  // Free the cgroup manager
  free_cgroup(manager);

  return 0;
}

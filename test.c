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
  unsigned int** collect_array = NULL;
  size_t collect_size = 0;
  for (int i = 0; i < 10;i++) {
    size_t data_size = 1024 * 1024;
    unsigned int* data =
        (unsigned int*)malloc(data_size * sizeof(unsigned int));
    if (data == NULL) {
      perror("Failed to allocate memory");
      exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < data_size; i++) {
      data[i] = 1;
    }
    for (size_t i = 0; i < 10; i++) {
      for (size_t j = 0; j < 1000000; j++) {
        if (i + 1 < data_size) {
          data[i] += data[i + 1];
        }
      }
    }
    collect_size++;
    unsigned int** temp = (unsigned int**)realloc(
        collect_array, collect_size * sizeof(unsigned int*));
    if (temp == NULL) {
      perror("Failed to realloc memory");
      free(data);
      for (size_t k = 0; k < collect_size - 1; k++) {
        free(collect_array[k]);
      }
      free(collect_array);
      exit(EXIT_FAILURE);
    }
    collect_array = temp;
    collect_array[collect_size - 1] = data;
    size += sizeof(unsigned int);
    printf("Allocated memory: %zu MB\n", size);
  }
}

int main() {
  struct Cgroup* cgroup = create_cgroup("my_cgroup");
  int pid = getpid();
  printf("Adding current process:%d to cgroup\n", pid);
  limit_cpu_usage(cgroup, 10);
  printf("Set CPU limit to 10%%\n");
  limit_mem_usage(cgroup, 1* 1024*1024);
  printf("Set memory limit to 1MB\n");
  add_pid(cgroup, pid);
  intensive_task();
  free_cgroup(cgroup);
  return 0;
}

### Rust Cgroups Library

This library provides a set of functions to manage control groups (cgroups) in Rust. Cgroups is a Linux kernel feature that limits, accounts for, and isolates the resource usage of a collection of processes.

### Features

- `create_cgroup(group_name: *const c_char) -> *mut Cgroup`: Create a new cgroup with the given name.
- `limit_cpu_usage(cgroup: *mut Cgroup, max_cpu: i64)`: Set the CPU usage limit for a cgroup.
- `limit_mem_usage(cgroup: *mut Cgroup, max_memory: i64)`: Set the memory usage limit for a cgroup.
- `add_pid(cgroup: *mut Cgroup, pid: u64)`: Add a process (PID) to a cgroup.
- `free_cgroup(cgroup: *mut Cgroup)`: Free the memory allocated for a cgroup.

### Usage

1. **Create a Cgroup:**
   ```rust
   let group_name = CString::new("test_group").expect("CString::new failed");
   let cgroup = create_cgroup(group_name.as_ptr());
   assert!(!cgroup.is_null());
   ```

2. **Limit CPU Usage:**
   ```rust
   limit_cpu_usage(cgroup, 50); // Set CPU limit to 50%
   ```

3. **Limit Memory Usage:**
   ```rust
   limit_mem_usage(cgroup, 1024 * 1024 * 1024); // Set memory limit to 1 GB
   ```

4. **Add PID to Cgroup:**
   ```rust
   add_pid(cgroup, 12345); // Add a fake PID for testing purposes
   ```

5. **Free Cgroup:**
   ```rust
   free_cgroup(cgroup); // Ensure this doesn't panic or leak memory
   ```

### Notes

- This library is intended for use on Linux systems.
- Ensure that your program has the necessary permissions to create and manage cgroups.
use cgroups_rs::cgroup_builder::*;
use cgroups_rs::*;
use std::ffi::CStr;
use std::os::raw::c_char;
use std::ptr::null_mut;

#[no_mangle]
pub extern "C" fn create_cgroup(group_name: *const c_char) -> *mut Cgroup {
    let group_name = unsafe {
        assert!(!group_name.is_null());
        CStr::from_ptr(group_name)
    };

    let group_name = match group_name.to_str() {
        Ok(name) => name,
        Err(_) => return null_mut(),
    };

    let hier = cgroups_rs::hierarchies::auto();
    match CgroupBuilder::new(group_name).build(hier) {
        Ok(cgroup) => Box::into_raw(Box::new(cgroup)),
        Err(e) => {
            eprintln!("Failed to create cgroup: {}", e);
            return null_mut();
        }
    }
}

//Specify a percentage quota of cpu time, eg:100 and -1 means 100% of cpu time.
#[no_mangle]
pub extern "C" fn limit_cpu_usage(cgroup: *mut Cgroup, max_cpu: i64) {
    let cgroup = unsafe {
        assert!(!cgroup.is_null());
        &mut *cgroup
    };

    if let Some(cpu_controller) = cgroup.controller_of::<cgroups_rs::cpu::CpuController>() {
        let cfs_period = match cpu_controller.cfs_period() {
            Ok(period) => period as i64,
            Err(e) => {
                eprintln!("Failed to get cfs_period: {}", e);
                return;
            }
        };
        cpu_controller
            .set_cfs_quota(cfs_period * max_cpu)
            .expect("Failed to set cfs_quota");
    }
}

//Set the memory usage limit of the control group, in bytes.
#[no_mangle]
pub extern "C" fn limit_mem_usage(cgroup: *mut Cgroup, max_memory: i64) {
    let cgroup = unsafe {
        assert!(!cgroup.is_null());
        &mut *cgroup
    };

    if let Some(mem_controller) = cgroup.controller_of::<cgroups_rs::memory::MemController>() {
        mem_controller
            .set_limit(max_memory)
            .expect("Failed to set memory limit");
    }
}

#[no_mangle]
pub extern "C" fn add_pid(cgroup: *mut Cgroup, pid: u64) {
    let cgroup = unsafe {
        assert!(!cgroup.is_null());
        &mut *cgroup
    };
    cgroup
        .add_task_by_tgid(CgroupPid::from(pid))
        .expect(&format!("Failed to add task to cgroup for pid: {}", pid));
}

#[no_mangle]
pub extern "C" fn free_cgroup(cgroup: *mut Cgroup) {
    if !cgroup.is_null() {
        unsafe {
            drop(Box::from_raw(cgroup));
        }
    }
}
#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CString;
    fn intensive_task() {
        let mut size = 0;
        let mut collect_vec = vec![];
        loop {
            let mut data = vec![1u32; 1024 * 1024];
            for i in 0..1024 {
                for _ in 0..1000000 {
                    data[i] += data[i + 1]
                }
            }
            collect_vec.push(data);
            size += 1;
            println!("Allocated memory: {} kb", size);
        }
    }
    #[test]
    fn test_limit_cpu_usage() {
        let group_name = CString::new("test_group").expect("CString::new failed");
        let cgroup = create_cgroup(group_name.as_ptr());
        assert!(!cgroup.is_null());
        limit_cpu_usage(cgroup, 50); // Set CPU limit to 50%
        intensive_task();

        // Clean up
        free_cgroup(cgroup);
    }

    #[test]
    fn test_limit_mem_usage() {
        let group_name = CString::new("test_group").expect("CString::new failed");
        let cgroup = create_cgroup(group_name.as_ptr());
        assert!(!cgroup.is_null());

        limit_mem_usage(cgroup, 1024 * 1024 * 1024); // Set memory limit to 1 GB
        intensive_task();

        // Clean up
        free_cgroup(cgroup);
    }
}

compile:
	cargo b && gcc -o call_rust test.c ./target/debug/libcgroups_ffi.a -lpthread -ldl 
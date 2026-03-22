# Compiling Rust for i586 architecture locally on Tiny Core Linux

## Context
I have a 64 bit Phenom II CPU based computer on which I installed Debian 13.3.
On a single partition, I have all the Debian system files.
I downloaded Tiny Core Linux to `/boottcl`

## The Rust Compilation for i586
`make build-locally` from [rust-i586](https://github.com/linic/rust-i586) should run without error and produce all the Rust tools which target i586.

## Possible Issues Preventing a Successful Compilation
### An Architecture Mismatch
I tried compiling using [rust-i586](https://github.com/linic/rust-i586) on Core Pure 64 bit and it didn't work.
When I `./cargo`, I saw an error message `-sh: ./cargo: not found`
I learned that the error usually does not mean the file is missing. In Linux shells, this message often indicates that a required interpreter or dynamic loader referenced by the binary cannot be found.
Since we are in core pure 64 bits binaries that are dynamically linked like `cargo` expect a loader such as `/lib64/ld-linux-x86-64.so.2`.
I was trying to run a 32 bits version of `cargo` on a pure 64 bits system so the kernel failed to start the binary and the shell printed “not found”.

If `file cargo` had returned something like `ELF 64-bit LSB executable, x86-64, dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2`, I would then have checked
`ls /lib64/ld-linux-x86-64.so.2` or similar to see if the loader existed.

If I ever need, I also learned that if the loader exists, required libraries might not and I can check with `ldd cargo`
and if the results show `not found` beside any libraries, those libraries must be installed.

Architecture mismatch was my issue. I could confirm this with `uname -m` and `file cargo`.
The system was x86_64, but `cargo` was 32 bit and searches for 32 bits libraries.
The kernel tried to start `cargo` using the 32-bit ELF interpreter (dynamic loader), it was missing and I got `-sh: ./cargo: not found`

A 32-bit ELF binary typically specifies the `/lib/ld-linux.so.2` interpreter which can be confirmed with `file cargo` which can output something similar to
`ELF 32-bit LSB executable, Intel 80386, dynamically linked, interpreter /lib/ld-linux.so.2`

Part of the solution was to use a a 64 bits kernel with a 32 bits user space.
I used `rootfs.gz` which is the 32 bits user space and `module64.gz` which goes with
the `vmlinuz64` kernel, I was on a multi-boot setup where `grub` was the boot loader.
I used a `menuentry` in `/etc/grub.d/40_custom` while in Debian 13.3.
I used `sudo vim /etc/grub.d/40_custom` and added
```
menuentry "core64 17.0 with 32 bits user space" {
	insmod ext2
	set root='hd0,1'
	linux /boottcl/vmlinuz64-17.0 vga=795 syslog showapps tce=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" home=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" opt=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" lang=fr_CA.UTF-8
	initrd /boottcl/rootfs-17.0.gz /boottcl/modules64-17.0.gz
}
```


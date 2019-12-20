# DracoVM Compiler

[![Build Status](https://travis-ci.org/pointbazaar/dracovm-compiler.svg?branch=master)](https://travis-ci.org/pointbazaar/dracovm-compiler)
[![CodeFactor](https://www.codefactor.io/repository/github/pointbazaar/dracovm-compiler/badge)](https://www.codefactor.io/repository/github/pointbazaar/dracovm-compiler)
[![GitHub license](https://img.shields.io/github/license/pointbazaar/dracovm-compiler.svg)](https://github.com/pointbazaar/dracovm-compiler/blob/master/LICENSE)

DracoVM Language is a **stack based intermediate language**. Meaning it is not meant to be written by humans.

The DracoVM Compiler compiles it to  **x86 Assembly Language**.

This is the compiler for the intermediate language produced by [dragon](https://github.com/pointbazaar/dragon/).

You can use it to implement your own Programming Language on top of it.

It is easily extensible. 

## Usage

```console
You@Terminal:~$ dracovm Main.subroutine.dracovm
```

## Goals

- [ ] fully incremental Compiler
- [ ] ability to link with object files from other programming languages

- [x] integer arithmetic support
- [x] integer comparison support

- [x] float arithmetic support
- [x] float comparison support

- [x] array support
- [x] length-prefixed arrays

- [ ] Builtin Subroutines in lexicographical order (mostly written in assembly) to execute linux system calls [syscalls](http://man7.org/linux/man-pages/man2/syscalls.2.html)
  - [x] sys_mmap as malloc(...) and new(...)
  - [ ] sys_gettid() as gettid() to get thread identfication
  - [ ] sys_kill(...) as kill(...) to kill processes
  - [ ] sys_lseek(...) as lseek(...) to reposition read/write offset
  - [ ] sys_mkdir(...) as mkdir(...) to create a new directory
  - [ ] sys_mount(...) as mount(...) 
  - [ ] sys_nice(...) as nice(...)
  - [x] sys_read as readchar(...) but should be implemented for files also
  - [ ] sys_readdir(...) as readdir(...)
  - [ ] sys_reboot(...) as reboot(...) to reboot the system
  - [ ] sys_rename(...) as rename(...) to rename a file and move it to another directory if neccessary
  - [ ] sys_stime(...) as stime(...) to set the time
  - [x] sys_time as time()
  - [x] sys_write as putchar(...) but should be implemented for files also
  - ...

- [ ] other Builtin Subroutines
  - [x] len(...) to calculate length of a block allocated with new(...)
## Contributions

Contributions are encouraged, but only in the Form of Issues, Suggestions and Forks. Pull Requests will probably not be accepted.

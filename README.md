# DracoVM Compiler

## Description

DracoVM Language is a **stack based intermediate language**.

This is the compiler for the intermediate language produced by [dragon](https://github.com/pointbazaar/dragon/).

It takes this stack-based intermediate language and compiles it to  **x86 Assembly Language**.

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

## Contributions

Contributions are encouraged, but only in the Form of Issues, Suggestions and Forks. Pull Requests will probably not be accepted.

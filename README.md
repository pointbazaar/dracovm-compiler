# DracoVM Compiler

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

## Contributions

Contributions are encouraged, but only in the Form of Issues, Suggestions and Forks. Pull Requests will probably not be accepted.

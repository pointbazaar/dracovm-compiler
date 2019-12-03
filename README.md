# DracoVM Compiler

[![CodeFactor](https://www.codefactor.io/repository/github/pointbazaar/dracovm-compiler/badge)](https://www.codefactor.io/repository/github/pointbazaar/dracovm-compiler)

[![Known Vulnerabilities](https://snyk.io/test/github/pointbazaar/dracovm-compiler/badge.svg)](https://snyk.io/test/github/pointbazaar/dracovm-compiler)

## Description

This is the compiler for the intermediate language produced by https://github.com/pointbazaar/dragon/.

It takes this stack-based intermediate language and compiles it to Intel x86 Assembly Language.

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

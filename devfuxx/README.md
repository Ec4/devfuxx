## What is devfuxx? ##

*devfuxx* is a Brainf\*ck compiler that behaves as a character device driver.


## Install ##

1. Build

```
$ make
```

2. Load Kernel Module

```
$ sudo insmod devfuxx.ko
```

## Usage ##

I wrote the parser example.
You can use this after compiling *bf\_compiler.c*.
```
$ cc bf_compiler.c -o bfc
$ ./bfc hello.bf
Hello world!
```

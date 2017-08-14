## What is devfuxx? ##

*devfuxx* is a Brainf\*ck interpreter that behaves as a character device driver.


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
$ cc bf_interpreter.c -o bfi
$ ./bfi hello.bf
Hello world!
```

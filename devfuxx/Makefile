obj-m := devfuxx.o

ROOTDIR  := /tmp/linux-4.12
PWD   := $(shell pwd)

default:
	$(MAKE) -C $(ROOTDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(ROOTDIR) M=$(PWD) modules clean

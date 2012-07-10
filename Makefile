# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := circular-char.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /home/karim/opersys-dev/android/kernel/qemu-x
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

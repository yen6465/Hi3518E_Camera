
ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../Makefile.param
	include $(PARAM_FILE)
endif

obj-m := ssp_ad9020.o
ssp-y += hi_ssp.o

ifeq ($(HIARCH),hi3518)
	export HI_ARCH?=3518
	EXTRA_CFLAGS += -DHI_ARCH=$(HI_ARCH)
endif

all:
	$(CC) -g -Wall -o ssp_read ssp_read.c strfunc.c	
	$(CC) -g -Wall -o ssp_write ssp_write.c	strfunc.c
	make -C $(LINUX_ROOT) M=$(PWD) modules

clean:
	make -C $(LINUX_ROOT) M=$(PWD) clean
	rm -f  ssp_read ssp_write


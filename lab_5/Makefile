obj-m += server.o

all:user.c
	gcc user.c -o user
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f user

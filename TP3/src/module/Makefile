obj-m += hello.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=/home/kalantos/Desktop modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=/home/kalantos/Desktop clean

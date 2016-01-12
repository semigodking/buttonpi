# Copyright Zhuofei Wang <semigodking@gmail.com>. All rights reserved.

all: buttonpi.c
	gcc -fPIC -lwiringPi -o buttonpi buttonpi.c

#!/bin/sh

if [ "$USER" ==  "root" ]; then
	insmod HRT.ko
	insmod Squeue.ko
else
	sudo insmod HRT.ko
	sudo insmod Squeue.ko
fi


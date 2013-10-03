#/bin/sh

if [ "$USER" == "root" ]; then
	rmmod Squeue
	rmmod HRT
else
	sudo rmmod Squeue
	sudo rmmod HRT
fi

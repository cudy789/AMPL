# Setup instructions

Git pull

git submodule update --init --recursive

./build-and-run.sh


### RPi video bandwidth throttling
[https://forums.raspberrypi.com/viewtopic.php?t=35689](https://forums.raspberrypi.com/viewtopic.php?t=35689)


rmmod uvcvideo

modprobe uvcvideo nodrop=1 timeout=5000
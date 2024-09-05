# Setup instructions

Git pull

git submodule update --init --recursive

./build-and-run.sh


### RPi video bandwidth throttling
[https://forums.raspberrypi.com/viewtopic.php?t=35689](https://forums.raspberrypi.com/viewtopic.php?t=35689)

Plug all cameras in, then run

sudo rmmod uvcvideo

sudo modprobe uvcvideo nodrop=1 timeout=5000

I think there are a couple things going on still:
USB2 power/bandwidth/overheating is causing the latency to slowly increase, creeping from 30fps on 3 cameras to 15 fps on 3 cams
Maybe try USB3 cameras to spread the load. CPU is totally fine, less than 50% utilization across all cores. ~25% now with low res.
# Gstreamer Streaming
This project can be used wrapper code for streaming of camera frames with very low latency.It can also be used to add multiple sinks.

## Getting Started
Download or clone this repository to your local Raspberry File System.

### Prerequisites

You need to have installed GStreamer on both source and target machines.This can be done using
```
sudo apt-get install gstreamer-1.0* 
```

### Installing
Download this project to your machine.
Move to the folder using
```
cd EmbeddedNotifier
```
And compile using
```
g++ -o streamer qstreamer.cpp qstreamer.h Test.cpp  `pkg-config --cflags --libs gstreamer-1.0`
```
Run this using 
```
./streamer
```
Provide the respective details.
## Target machine viewing frames
Use gstreamer pipeline mentioned below to view the camera stream:
```
gst-launch-1.0 -v udpsrc port=9000 ! "application/x-rtp, payload=96" ! rtpjpegdepay ! jpegdec ! autovideosink

```
## Authors

* **Amanpreet Walia** 

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
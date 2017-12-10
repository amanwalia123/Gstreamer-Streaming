#include "qstreamer.h"

int main(){
    QStreamer *streamer = new QStreamer();
    if(!streamer->createElements())
    {
        printf("GStreamer failed", "GStreamer failed to create pipeline elements!");
        return;
    }
    streamer->setCameraID(0); //using inbuilt laptop camera
    
    streamer->addClient("192.168.0.13:9000") // Format of IP address is "<IPAddress of machine>:<Port#>"
                                             //Multiple clients can be added in similar format
    streamer->initialize();
    streamer->startStreaming();
    return 0;
}

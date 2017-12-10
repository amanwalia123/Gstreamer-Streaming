#ifndef QSTREAMER_H
#define QSTREAMER_H

#include <glib.h>
#include <gst/gst.h>
#include <iostream>

/*Network Information to stream on board*/
#define MY_IP "127.0.0.1"
#define MY_PORT 5000

/* Video Parameters */
#define IMG_WITDH 640
#define IMG_HEIGHT 480

class QStreamer
{
private:
    GMainLoop *loop;
    /* Elements of the pipeline */
    GstElement *pipeline, *source , *payLoad,*sink;
    /* Bus to put all the connected elements on */
    GstBus *bus;
    guint bus_watch_id;
    /* Client IP address and Port */
    std::string clientAddr;
    /* camera id to stream video from */
    int camID;


public:
    QStreamer();
    ~QStreamer();
    /* set the streaming pipeline to ON state */
    void startStreaming();
    /* set the streaming pipeline to OFF state */
    void stopStreaming();
    /* create gstreamer elements */
    bool createElements();
    /* Initialize the process of streaming by setting the parameters accordingly*/
    void initialize();
    /* Add IP address and port number for each client to stream the video to */
    void addClient(std::string Address);
    /*camera number for v4l2src keeps changing on new connection, so this method sets camera id if by chance camera id is changed */
    void setCameraID(int id);
    /* getting error messages from bus */
    static gboolean bus_call (GstBus *bus,GstMessage *msg,gpointer data);


};

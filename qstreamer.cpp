#include "qstreamer.h"
#include <string>
#include <sstream>


QStreamer::QStreamer():
    clientAddr(""),
    camID(-1)
{

}

bool QStreamer::createElements(){

    /* Initialisation */
    gst_init (NULL, NULL);
    loop = g_main_loop_new (NULL, FALSE);

    /* Create gstreamer elements */
    pipeline = gst_pipeline_new ("video-stream");
    source   = gst_element_factory_make ("v4l2src", "camera-source");
    payLoad =  gst_element_factory_make ("rtpjpegpay","rtp jpeg payloader");
    sink     = gst_element_factory_make ("multiudpsink", "video-output");

    if (!pipeline) {
        g_printerr ("Pipeline could not be created. Exiting.\n");
        return false;
    }

    if (!source) {
        g_printerr ("source could not be created. Exiting.\n");
        return false;
    }

    if (!payLoad) {
        g_printerr ("payLoad could not be created. Exiting.\n");
        return false;
    }

    if (!sink) {
        g_printerr ("Sink could not be created. Exiting.\n");
        return false;
    }

    return true;
}

void QStreamer::setCameraID(int camID){

    if(pipeline == NULL )
        this->camID = camID;
    else {
        GstState state;
        gst_element_get_state (pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
        if(state == GST_STATE_PLAYING || state == GST_STATE_READY){
            gst_element_set_state(pipeline,GST_STATE_NULL);
            this->camID = camID;
            initialize();
            gst_element_set_state(pipeline,state);
        }
        else if(state == GST_STATE_NULL){
            this->camID = camID;
        }
    }


    std::cout<<"camera is:"<<this->camID<<std::endl;
}

void QStreamer::addClient(std::string addr){

    if(pipeline == NULL ){
        if(!clientAddr.empty())
            clientAddr.append(",");
        clientAddr.append(addr);
    }
    else {
        GstState state;
        gst_element_get_state (pipeline, &state, NULL, GST_CLOCK_TIME_NONE);
        if(state == GST_STATE_PLAYING || state == GST_STATE_READY){
            gst_element_set_state(pipeline,GST_STATE_NULL);

            /* wait until it's changed or failed */
            if (gst_element_get_state (pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
                g_error ("Failed to go into NULL state");
            }

            if(!clientAddr.empty())
                clientAddr.append(",");

            clientAddr.append(addr);

            initialize();
            gst_element_set_state(pipeline,state);
            /* wait until it's changed or failed */
            if (gst_element_get_state (pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
                g_error ("Failed to go into initial state");
            }

        }
        else if(state == GST_STATE_NULL){
            if(!clientAddr.empty())
                clientAddr.append(",");
            clientAddr.append(addr);
        }
    }
    std::cout<<"Address is:"<<clientAddr<<std::endl;

}

void QStreamer::initialize(){
    /* Set up the pipeline */

    /* we set the camera device */
    std::string v4lsource = "/dev/video";

    std::stringstream ss;
    ss << camID;

    v4lsource.append( ss.str());
    std::cout<<"v4lsource is:"<<v4lsource<<std::endl;

    g_object_set (G_OBJECT (source), "device", v4lsource.c_str(), NULL);

    /* setting udp sink parameters */
    g_object_set(G_OBJECT(sink), "clients", clientAddr.c_str(), NULL);

    /* set gstreamer bus */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    gst_bin_add_many (GST_BIN (pipeline), source,payLoad, sink, NULL);

    GstCaps *caps = NULL;
    caps = gst_caps_from_string("image/jpeg,width=960,height=540,videorate=8/1");
    gst_element_link_filtered(source,payLoad,caps);
    gst_caps_unref(caps);
    gst_element_link_many (payLoad, sink, NULL);
}

void QStreamer::startStreaming(){

    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    if (gst_element_get_state (pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
        g_error ("Failed to go into playing state");
    }
}

void QStreamer::stopStreaming(){

    gst_element_set_state (pipeline, GST_STATE_NULL);
    if (gst_element_get_state (pipeline, NULL, NULL, -1) == GST_STATE_CHANGE_FAILURE) {
        g_error ("Failed to go into stop state");
    }
}

gboolean QStreamer:: bus_call(GstBus *bus,GstMessage *msg,gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
        g_print ("End of stream\n");
        g_main_loop_quit (loop);
        break;

    case GST_MESSAGE_ERROR: {
        gchar  *debug;
        GError *error;
        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);

        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);

        g_main_loop_quit (loop);
        break;
    }
    default:
        break;
    }

    return TRUE;
}

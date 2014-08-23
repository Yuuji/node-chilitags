#ifndef CHILITAGS_H
#define CHILITAGS_H

#include <node.h>

// The Chilitags header
#include <chilitags/chilitags.hpp>

#ifdef OPENCV3
#include <opencv2/core/utility.hpp> // getTickCount...
#endif

#include <opencv2/core/core_c.h> // CV_AA
#include <opencv2/highgui/highgui.hpp>

struct Border {
    cv::Point p1;
    cv::Point p2;
};

struct ChilitagInfo {
    int id;
    Border borders[4];
    cv::Point2f center;
    ChilitagInfo *next = NULL;
};

struct ChilitagData {
    ChilitagInfo *tags = NULL;
    cv::Mat inputImage;
    double processingTime;
};

class Chilitags : public node::ObjectWrap {
public:
    static void Init(v8::Handle<v8::Object> exports);
    
private:
    explicit Chilitags(int setxRes = 640, int setyRes = 480, int setcameraIndex = 0);
    ~Chilitags();
    
    struct ChilitagData* detectChilitags();
    
    static v8::Handle<v8::Value> New(const v8::Arguments& args);
    static v8::Handle<v8::Value> detect(const v8::Arguments& args);
    static v8::Persistent<v8::Function> constructor;
    
    int xRes = 640;
    int yRes = 480;
    int cameraIndex = 0;
    
    cv::VideoCapture capture;
    // The tag detection happens in the Chilitags class.
    chilitags::Chilitags chilitags;
};

#endif

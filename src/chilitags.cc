#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "chilitags.h"

using namespace v8;

Persistent<Function> Chilitags::constructor;

Chilitags::Chilitags(int setxRes, int setyRes, int setcameraIndex) {
    xRes = setxRes;
    yRes = setyRes;
    cameraIndex = setcameraIndex;
    
    cv::VideoCapture capture(cameraIndex);
    if (!capture.isOpened())
    {
	    ThrowException(Exception::TypeError(String::New("Unable to initalise video capture.")));
        return;
    }
    
#ifdef OPENCV3
    capture.set(cv::CAP_PROP_FRAME_WIDTH, xRes);
    capture.set(cv::CAP_PROP_FRAME_HEIGHT, yRes);
#else
    capture.set(CV_CAP_PROP_FRAME_WIDTH, xRes);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, yRes);
#endif
    
    chilitags.setFilter(0, 0.);
}

Chilitags::~Chilitags() {
    capture.release();
}

void Chilitags::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
    tpl->SetClassName(String::NewSymbol("Chilitags"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    // Prototype
    tpl->PrototypeTemplate()->Set(String::NewSymbol("detect"),
                                  FunctionTemplate::New(detect)->GetFunction());
    constructor = Persistent<Function>::New(tpl->GetFunction());
    exports->Set(String::NewSymbol("Chilitags"), constructor);
}

Handle<Value> Chilitags::New(const Arguments& args) {
    HandleScope scope;
    
    if (args.IsConstructCall()) {
        // Invoked as constructor: `new Chilitags(...)`
        int xRes = args[0]->IsUndefined() ? 640 : args[0]->NumberValue();
        int yRes = args[1]->IsUndefined() ? 480 : args[1]->NumberValue();
        int cameraIndex = args[2]->IsUndefined() ? 0 : args[2]->NumberValue();
        Chilitags* obj = new Chilitags(xRes, yRes, cameraIndex);
        obj->Wrap(args.This());
        return args.This();
    } else {
        // Invoked as plain function `Chilitags(...)`, turn into construct call.
        const int argc = 3;
        Local<Value> argv[argc] = { args[0], args[1], args[2] };
        return scope.Close(constructor->NewInstance(argc, argv));
    }
}

struct ChilitagData* Chilitags::detectChilitags() {
    struct ChilitagData* data = new ChilitagData;
    // Capture a new image.
    capture.read(data->inputImage);
    
    // Start measuring the time needed for the detection
    int64 startTime = cv::getTickCount();
    
    // Detect tags on the current image (and time the detection);
    // The resulting map associates tag ids (between 0 and 1023)
    // to four 2D points corresponding to the corners positions
    // in the picture.
    std::map<int, chilitags::Quad> tags = chilitags.find(data->inputImage);
    
    // Measure the processing time needed for the detection
    int64 endTime = cv::getTickCount();
    data->processingTime = 1000.0*((double) endTime - startTime)/cv::getTickFrequency();
    
    
    // Now we start using the result of the detection.
    
    // First, we set up some constants related to the information overlaid
    // on the captured image
    const static cv::Scalar COLOR(255, 0, 255);
    // OpenCv can draw with sub-pixel precision with fixed point coordinates
    static const int SHIFT = 16;
    static const float PRECISION = 1<<SHIFT;
    
    cv::flip(data->inputImage,data->inputImage,1);
    
    int count = 0;
    struct ChilitagInfo *currentTag;

    for (const std::pair<int, chilitags::Quad> & tag : tags) {
        int id = tag.first;
        // We wrap the corner matrix into a datastructure that allows an
        // easy access to the coordinates
        const cv::Mat_<cv::Point2f> corners(tag.second);
        
        count++;
        if (count==1) {
            data->tags = new ChilitagInfo;
            currentTag = data->tags;
        } else {
            currentTag->next = new ChilitagInfo;
            currentTag = currentTag->next;
        }
        
        for (size_t i = 0; i < 4; ++i) {
            cv::Point p1 = corners(i);
            cv::Point p2 = corners((i+1)%4);
            p1.x = xRes - p1.x;
            p2.x = xRes - p2.x;
            
            currentTag->borders[i].p1 = p1;
            currentTag->borders[i].p2 = p2;
        }
        
        cv::Point center1 = corners(0);
        cv::Point center2 = corners(1);
        
        center1.x = xRes - center1.x;
        center2.x = xRes - center2.x;
        
        currentTag->center = 0.5*(center1 + center2);
        currentTag->id = id;
    }
    
    return data;
}

Handle<Value> Chilitags::detect(const Arguments& args) {
    HandleScope scope;
    
    Chilitags* obj = ObjectWrap::Unwrap<Chilitags>(args.This());
    struct ChilitagData* data = obj->detectChilitags();
    
    Handle<Object> Result = Object::New();
    Result->Set(String::New("processingTime"), Number::New(data->processingTime));
    
    int key = 0;
    
    Handle<Object> tags = Object::New();
    
    ChilitagInfo *tag = data->tags;
    ChilitagInfo *lasttag;
    while (tag) {
        Handle<Object> tagObject = Object::New();
        tagObject->Set(String::New("id"), Number::New(tag->id));
        
        Handle<Object> borders = Object::New();
        for (int i=0; i<4; i++) {
            Handle<Object> border = Object::New();
            
            Handle<Object> p1 = Object::New();
            p1->Set(String::New("x"), Number::New(tag->borders[i].p1.x));
            p1->Set(String::New("y"), Number::New(tag->borders[i].p1.y));
            border->Set(String::New("p1"), p1);
            
            Handle<Object> p2 = Object::New();
            p2->Set(String::New("x"), Number::New(tag->borders[i].p2.x));
            p2->Set(String::New("y"), Number::New(tag->borders[i].p2.y));
            border->Set(String::New("p2"), p2);
            
            borders->Set(Number::New(i), border);
        }
        tagObject->Set(String::New("borders"), borders);
        
        Handle<Object> center = Object::New();
        center->Set(String::New("x"), Number::New(tag->center.x));
        center->Set(String::New("y"), Number::New(tag->center.y));
        tagObject->Set(String::New("center"), center);
        
        lasttag = tag;
        tag = tag;
        
        delete lasttag;
        
        tags->Set(Number::New(key), tagObject);
        key++;
    }
    
    Result->Set(String::New("tags"), tags);
    
    delete data;
    return scope.Close(Result);
}

{
    "targets": [
        {
            "target_name": "node-chilitags",
            "sources": [ "chilitags.cc", "node-chilitags.cc" ],
            "libraries": [ "-lchilitags -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect" ],
            "cflags": [
                "-std=c++11"
            ],
           "xcode_settings": {
               "OTHER_CFLAGS": [
                   "-std=c++11"
               ],
            },
        }
    ]
}

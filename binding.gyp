{
    "targets": [
        {
            "target_name": "node-chilitags",
            "sources": [ "chilitags.cc", "node-chilitags.cc" ],
            "libraries": [ "-lchilitags -lopencv" ],
            "cflags": [
                "-std=c++11",
                "-stdlib=libc++"
            ],
           "xcode_settings": {
               "OTHER_CFLAGS": [
                   "-std=c++11"
               ],
            },
        }
    ]
}

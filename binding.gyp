{
    "targets": [
        {
            "target_name": "chilitags",
            "sources": [ "src/base64.cc", "src/chilitags.cc", "src/node-chilitags.cc" ],
            "libraries": [ "-lchilitags -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect" ],
            "cflags": [
                "-std=c++11"
            ],
            "xcode_settings": {
                "GCC_ENABLE_CPP_RTTI": "YES",
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "OTHER_CPLUSPLUSFLAGS":["-stdlib=libc++"],
                "OTHER_LDFLAGS":["-stdlib=libc++", "-L/opt/local/lib"],
                "CLANG_CXX_LANGUAGE_STANDARD":"c++11",
                "MACOSX_DEPLOYMENT_TARGET":"10.7"
            }
        }
    ]
}

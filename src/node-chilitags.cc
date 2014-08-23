#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "chilitags.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
    Chilitags::Init(exports);
}

NODE_MODULE(chilitags, InitAll)

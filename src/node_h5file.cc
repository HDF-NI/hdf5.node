#include <node.h>
#include <string>

#include "H5Cpp.h"
#include "node_h5file.h"

using namespace v8;
using namespace H5;
using namespace NodeHDF5;

File::File (std::string path) {

    file = new H5File(path, H5F_ACC_RDONLY);
    
}

File::~File () {
    
    delete file;
    
}

void File::Initialize (Handle<Object> target) {
    
    Local<String> class_name = String::NewSymbol("H5File");

    // instantiate constructor template
    Local<FunctionTemplate> plate = FunctionTemplate::New(New);
    
    // initialize template properties
    plate->SetClassName(class_name);
    plate->InstanceTemplate()->SetInternalFieldCount(1);
    
    // get context function instance
    Persistent<Function> constructor = Persistent<Function>::New(plate->GetFunction());
    
    // specify constructor function
    target->Set(class_name, constructor);
    
}

Handle<Value> File::New (const Arguments& args) {
    
    HandleScope scope;
    
    // get path argument
    String::Utf8Value path(args[0]->ToString());
    
    // create file object
    File* f = new File(std::string(*path));
    f->Wrap(args.This());
    
    // return file object
    return scope.Close(args.This());
    
}
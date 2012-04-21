#include <node.h>
#include <string>

#include "H5Cpp.h"

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    
    class File : public ObjectWrap {
        
        private:
            H5::H5File* file;
        
        public:
            File(std::string path);
            ~File();
            
            static void Initialize (Handle<Object> target);
            static Handle<Value> New (const Arguments& args);
        
    };

};
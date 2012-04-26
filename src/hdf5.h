#include <node.h>
#include <string>

#include "H5Cpp.h"

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    
    class File : public ObjectWrap {
        
        public:
            H5::H5File* m_file;
            
            File(std::string path);
            ~File();
            
            static void Initialize (Handle<Object> target);
            static Handle<Value> New (const Arguments& args);
            static Handle<Value> OpenGroup (const Arguments& args);
        
    };
    
    class Group : public ObjectWrap {
        
        private:
            H5::Group m_group;
            static Persistent<Function> Constructor;
            static Handle<Value> New (const Arguments& args);
            
        public:
            Group(H5::Group group);
            static void Initialize ();
            static Handle<Value> Instantiate (const Arguments& args);
        
    };

};
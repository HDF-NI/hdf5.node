#include <node.h>
#include <string>

#include "H5Cpp.h"

namespace NodeHDF5 {

    using namespace v8;
    using namespace node;
    
    class File : public ObjectWrap {
        
        private:
            H5::H5File* m_file;
        
        public:
            File(std::string path);
            ~File();
            
            static void Initialize (Handle<Object> target);
            static Handle<Value> New (const Arguments& args);
            static Handle<Value> Close (const Arguments& args);
        
    };
    
    class Group : public ObjectWrap {
            
        private:
            H5::Group* m_group;
        
        public:
            Group();
            ~Group();
            
            static void Initialize (Handle<Object> target);
            static Handle<Value> New (const Arguments& args);
        
    };

};
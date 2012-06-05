#pragma once
#include "DataFile.h"
#include "String.h"
#include <sstream>

class DataWriter : public DataFile
{
    public:
        DataWriter(const std::string &fileName);

        void Write(int data, const size_t size);
        void Write(const char* data, const size_t size);
        void Write(const char data);


    private:

        //std::ofstream *fileStream;
};

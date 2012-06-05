#pragma once
#include "datafile.h"

class DataReader : public DataFile
{
    public:
        DataReader(const std::string &fileName);

        char ReadByte(void) const;
        short ReadShort(void) const;
        int ReadInt(void) const;

        std::ifstream::pos_type GetFileSize(void) const;
        void SetPos(const std::ifstream::pos_type pos);
    private:
        //std::ifstream *fileStream;
        std::ifstream::pos_type fileSize;
};



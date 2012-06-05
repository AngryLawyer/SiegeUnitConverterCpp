#pragma once
#include <string>
#include <iostream>
#include <fstream>

class DataFile
{
    public:
        DataFile(void){};
        DataFile(const std::string &fileName);
        ~DataFile(void);

        virtual bool IsOpen(void) const;
        virtual void Close(void);
        static bool FileExists(const std::string &fileName);
    protected:
        std::string fileName;
        std::fstream *fileStream;
    private:
        DataFile(const DataFile &copied);

};

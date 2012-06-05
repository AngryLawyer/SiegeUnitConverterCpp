#include "StdAfx.h"
#include ".\datafile.h"
#include <sys/stat.h>

DataFile::DataFile(const std::string &fileName)
{
    this->fileName = fileName;
}

DataFile::~DataFile(void)
{
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            fileStream->flush();
            fileStream->close();
        }
        delete fileStream;
    }

}

void DataFile::Close(void)
{
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            fileStream->close();
        }
    }
}

bool DataFile::IsOpen(void) const
{
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            return true;
        }
    }
    return false;
}


bool DataFile::FileExists(const std::string &fileName)
{
    struct stat fileInfo;
    return (! stat(fileName.c_str(),&fileInfo)); //If the file is writeable, stat returns 0, so invert the result
}

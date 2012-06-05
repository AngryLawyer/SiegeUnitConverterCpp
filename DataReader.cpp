#include "StdAfx.h"
#include ".\datareader.h"

using namespace std;

DataReader::DataReader(const std::string &fileName)
{
    this->fileName = fileName;
    fileSize = 0;
    fileStream = new fstream(fileName.c_str(), ios::in|ios::binary|ios::ate);
    if (fileStream)
    {
        fileSize = fileStream->tellg();
        fileStream->seekg (0, ios::beg); //Wind back to the start of the file when opening a reader
    }
}


char DataReader::ReadByte(void) const
{
    if (fileStream)
    {
        if (fileStream->tellg() < fileSize)
        {
            return fileStream->get();
        }
    }
    return 0;
}
short DataReader::ReadShort(void) const
{
    if (fileStream)
    {
        if (fileStream->tellg() < static_cast<long>(fileSize) - 2)
        {
            short returned;
            fileStream->read(reinterpret_cast<char*>(&returned),sizeof(returned));
            return returned;
        }
    }
    return 0;
}
int DataReader::ReadInt(void) const
{
    if (fileStream)
    {
        if (fileStream->tellg() < static_cast<long>(fileSize) - 4)
        {
            int returned;
            fileStream->read(reinterpret_cast<char*>(&returned),sizeof(returned));
            return returned;
        }
    }
    return 0;
}

std::ifstream::pos_type DataReader::GetFileSize(void) const
{
    return fileSize;
}

void DataReader::SetPos(const std::ifstream::pos_type pos)
{
    if (pos >= 0 && pos < fileSize)
    {
        fileStream->seekg(pos);
    }
}

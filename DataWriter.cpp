#include "StdAfx.h"
#include ".\datawriter.h"


using namespace std;

DataWriter::DataWriter(const std::string &fileName)
{
    this->fileName = fileName;
    fileStream = NULL;
    //Initialize the filestream
    fileStream = new fstream(fileName.c_str(), ios::out|ios::binary|ios::trunc);
}

void DataWriter::Write(int data, const size_t size)
{
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            int sizeCount = 0;
            while (data > 0)
            {
                fileStream->put(char(data%256));
                data /= 256;
                ++sizeCount;
            }
            while (sizeCount < size) //Fill the remaining characters
            {
                fileStream->put(char(0));
                ++sizeCount;
            }      
        }
    }
}

void DataWriter::Write(const char data)
{
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            fileStream->put(data);
        }
    }
}

void DataWriter::Write(const char* data, const size_t size)
{
    if (!data)
    {
        std::cout << "Warning: attempted to write null pointer\n";
        return;
    }
    if (fileStream)
    {
        if (fileStream->is_open())
        {
            if (strlen(data) > size)
            {
                cout << "Warning: Attempting to write data to area larger than specified size\n";
                return;
            }

            fileStream->write(data,strlen(data));

            if (strlen(data) < size)
            {
                for (unsigned int i = 0; i < size - strlen(data); ++i)
                {
                    fileStream->put(char(0));//The files we're dealing with are little-endian, so fill after the placement of the data
                }        
            }

        }
    }
}


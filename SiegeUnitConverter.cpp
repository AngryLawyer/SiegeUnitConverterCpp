// SiegeUnitConverter.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SiegeUnitConverter.h"
#include "Datawriter.h"
#include "Datareader.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

const char siegeHeader[] = {0x08,0x01,0x20,0x00,0x10,0x10};  

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 2)
    {
        cout << "Error - you need to supply a file to open!\n";
        return -1;
    }

    //Do some basic checks on the filename's size
    string fileName = argv[1];
    if (fileName.length() < 5)
    {
        cout << "Error - bad file name\n";
        return -1;
    }

    string extension = fileName.substr(fileName.length() - 4,4);
    transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

    if (extension.compare(".BMP") == 0)
    {
        //Transform into an MUT of the same name
        return ConvertBmpToMut(fileName);
    }
    else if (extension.compare(".MUT") == 0)
    {
        //Transform into a BMP of the same name
        return ConvertMutToBmp(fileName);
    }
    else
    {
        cout << "Error - unsupported extension.\n";
    }

    return 0;
}

int ConvertMutToBmp(const string &fileName)
{
    DataReader mutFile(fileName);

    if (mutFile.IsOpen())
    {   
        //Check it's the right file type.
        char actualHeader[6];   
        memset(actualHeader,0,sizeof(actualHeader));

        for (int i = 0; i < 6; ++i)
        {
            actualHeader[i] = mutFile.ReadByte();
        }

        if (strcmp(actualHeader, siegeHeader) != 0) //The same header files - we're good to process them.
        {
            cout << "Error: not a valid MUT file\n";
            mutFile.Close();
            return -1;
        }

        char imageData[8192];
        memset(imageData,0,sizeof(imageData));
        for (int i = 8191; i >= 0; --i)  //Siege stores its images upside-down, so reverse it here.
        {
            imageData[i] = mutFile.ReadByte();
        }
        mutFile.Close();

        DataReader palFile("siege.pal");

        char* palette = NULL;
        if (palFile.IsOpen()) //Read our palette information
        {
            if (static_cast<int>(palFile.GetFileSize()) == 768)
            {
                palette = new char[palFile.GetFileSize()];
                for (int i = 0; i < palFile.GetFileSize(); ++i)
                {
                    palette[i] = palFile.ReadByte();
                }
                palFile.Close();
            }
            else
            {
                cout << "Error: Pallete incorrect size\n";
                return -1;
            }
        }
        else
        {
            cout << "Error: Failed reading Siege.pal\n";
            return -1;
        }

        //Start building a BMP file
        size_t lastBackslash = fileName.find_last_of("/\\");
        string newFileName = fileName.substr(lastBackslash+1, fileName.length() - lastBackslash - 4);
        newFileName+="bmp";

        if (DataFile::FileExists(newFileName))
        {
            cout << "Error: " << newFileName << " already exists - remove it before proceeding\n";
            return -1;
        }

        DataWriter bmpFile(newFileName);

        if (bmpFile.IsOpen())
        {
            bmpFile.Write("BM", 2); //BMP file format
            bmpFile.Write(9270,4);//File size
            bmpFile.Write(0,2); //Unused
            bmpFile.Write(0,2); //Unused
            bmpFile.Write(1078,4);//Offset
            //IMAGE HEADER
            bmpFile.Write(40,4);//header size
            bmpFile.Write(16,4);//Width
            bmpFile.Write(512,4);//Height
            bmpFile.Write(1,2);//Colour planes
            bmpFile.Write(8,2);//Colour depth - 256 colour
            bmpFile.Write(0,4);//Compression
            bmpFile.Write(8192,4); //Image size
            bmpFile.Write(0,4); //Horizontal resolution
            bmpFile.Write(0,4); //Vertical resolution
            bmpFile.Write(0,4); //colour pallette count - 0 means autodetect
            bmpFile.Write(0,4); //important colours
            //PALETTE
            for(int i = 0; i < 256; ++i)
            {
                bmpFile.Write(palette[(i*3)+2]); //Red   //Colours are stored as BGR in Siege's palette - backwards again
                bmpFile.Write(palette[(i*3)+1]); //Green
                bmpFile.Write(palette[(i*3)]); //Blue

                bmpFile.Write(0); //Unused - bitmaps pad the alpha channel to keep things in a nice DWORD size
            }
            //IMAGE DATA
            for (int j = 0; j < 8192; ++j)
            {
                bmpFile.Write(imageData[j]);
            }
        }
        else
        {
            cout << "Error: could not open " << newFileName << " for writing\n";      
            if (palette)
            {
                delete[] palette;
            }
            return -1;
        }

        if (palette)
        {
            delete[] palette;
        }
        cout << "Successfully created " << newFileName << "\n";
        return 0;
    }
    else
    {
        cout << "Error - could not open " << fileName << " for reading\n";
        return -1;
    }
}


int ConvertBmpToMut(const string &fileName)
{
    DataReader bmpFile(fileName);
    if (bmpFile.IsOpen())
    {
        short identifier = bmpFile.ReadShort();
        int fileSizePredicted = bmpFile.ReadInt();

        if (identifier != 0x4D42) //BM, as a little-endian DWORD
        {
            cout << "Error: Header incorrect - are you sure it's a valid bitmap?\n";
            bmpFile.Close();
            return -1;
        }
        if (static_cast<int>(bmpFile.GetFileSize()) != 9270 || fileSizePredicted != 9270) //256-colour bitmap files with Siege units in them will always be 9270 bytes.
        {
            cout << "Error: File size incorrect\n";
            bmpFile.Close();
            return -1;
        }

        bmpFile.SetPos(14); //Start of header file

        int headerSize = bmpFile.ReadInt();
        if (headerSize != 40) //If the header size isn't 40, something bad's happening.
        {
            cout << "Error: invalid header size\n";
            bmpFile.Close();
            return -1;
        }

        int width = bmpFile.ReadInt();
        int height = bmpFile.ReadInt();

        if (width != 16 || height != 512) //512/16 gives us 32 frames of 16x16 siege unit
        {
            cout << "Error: Image dimensions incorrect\n";
            bmpFile.Close();
            return -1;
        }

        bmpFile.SetPos(28);
        short colourDepth = bmpFile.ReadShort();

        if (colourDepth != 8)
        {
            cout << "Error: Must be a 256-colour image\n";
            bmpFile.Close();
            return -1;
        }

        bmpFile.SetPos(1078); //Jump to the start of image data.

        char imageData[8192];
        memset(imageData,0,sizeof(imageData));

        for (int i = 8191; i >= 0; --i) //Siege units are stored upside down, so we read our bitmap into the array upside down
        {
            imageData[i] = bmpFile.ReadByte();
        }
        bmpFile.Close();

        //We can start writing!
        size_t lastBackslash = fileName.find_last_of("/\\");
        string newFileName = fileName.substr(lastBackslash+1, fileName.length() - lastBackslash - 4);
        newFileName+="mut";

        if (DataFile::FileExists(newFileName))
        {
            cout << "Error: " << newFileName << " already exists - remove it before proceeding\n";
            return -1;
        }

        DataWriter mutFile(newFileName);
        if (mutFile.IsOpen())
        {
            for (int i = 0; i < sizeof(siegeHeader); ++i)
            {
                mutFile.Write(siegeHeader[i]); //We could have written this as a block, but it strangely nulls certain characters.
            }

            for (int i = 0; i < sizeof(imageData); ++i)
            {
                mutFile.Write(imageData[i]); //Finally write our image data
            }
        }
        else
        {
            cout << "Error: Could not open " << newFileName << " for writing\n";
            bmpFile.Close();
            return -1;
        }
        cout << "Successfully created " << newFileName << "\n";
    }
    else
    {
        cout << "Error: Could not open " << fileName << " for reading\n";
        return -1;
    }

    return 0;
}

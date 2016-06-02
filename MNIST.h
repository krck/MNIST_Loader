//  MNIST.h
/*************************************************************************
 * MNIST Dataset Loader
 *------------------------------------------------------------------------
 * Copyright (c) 2016 Peter Baumann
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 *************************************************************************/
//--------------------------------------------------------------------------------------
// http://yann.lecun.com/exdb/mnist/
// Yann LeCun (Courant Institute, NYU), Corinna Cortes (Google Labs, New York)
// and Christopher J.C. Burges (Microsoft Research, Redmond)
//
// The MNIST database of handwritten digits, has a training set of 60,000 examples,
// and a test set of 10,000 examples. It is a subset of a larger set available from NIST.
// The digits have been size-normalized and centered in a fixed-size image (28x28)
//--------------------------------------------------------------------------------------

#pragma once
#ifndef MNIST_h
#define MNIST_h

#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// BIG-Endian to LITTLE-Endian byte swap
#define swap16(n) (((n&0xFF00)>>8)|((n&0x00FF)<<8))
#define swap32(n) ((swap16((n&0xFFFF0000)>>16))|((swap16(n&0x0000FFFF))<<16))

typedef unsigned char           byte;
typedef std::vector<byte>       row;



struct MNISTchar {
    std::vector<row> pixelData;
    int label;
    MNISTchar(byte rows) : pixelData(std::vector<row>(rows)), label(0) {}
};



class MNIST {
public:
    const std::vector<MNISTchar> trainingData;  // Set of 60.000 handwritten digits to train the net
    const std::vector<MNISTchar> testData;      // Set of 10.000 different handwritten digits to test the net
    
    
    MNIST(const std::string& path)
        :   trainingData(getMNISTdata(path + "train-images.idx3-ubyte", path + "train-labels.idx1-ubyte")),
            testData(getMNISTdata(path + "t10k-images-idx3-ubyte", path + "t10k-labels-idx1-ubyte")) {}
    
    
private:
    std::vector<MNISTchar> getMNISTdata(const std::string& imagepath, const std::string& labelpath) {
        std::vector<MNISTchar> tmpdata = std::vector<MNISTchar>();
        std::fstream file (imagepath, std::ifstream::in | std::ifstream::binary);
        int magicNum_images = 0, magicNum_labels = 0;
        int itemCount_images = 0, itemCount_labels = 0;
        // READ THE IMAGE FILE DATA
        if(file.is_open()) {
            int row_count = 0, col_count = 0;
            // FILE HEADER INFO is stored as 4 Byte Integers
            file.read((char*)&magicNum_images, 4);
            file.read((char*)&itemCount_images, 4);
            file.read((char*)&row_count, 4);
            file.read((char*)&col_count, 4);
            // Transform Byte values from big to little endian
            magicNum_images = swap32(magicNum_images);
            itemCount_images = swap32(itemCount_images);
            row_count = swap32(row_count);
            col_count= swap32(col_count);
            // Loop throug all the items and store every pixel of every row
            for (int i = 0; i < itemCount_images; i++) {
                MNISTchar tmpchar = MNISTchar(row_count);
                for(int r = 0; r < row_count; r++) {
                    for(int c = 0; c < col_count; c++) {
                        byte pixel = 0;
                        // read one byte (0-255 color value of the pixel)
                        file.read((char*)&pixel, 1);
                        tmpchar.pixelData[r].push_back(pixel);
                    }
                }
                tmpdata.push_back(tmpchar);
            }
        }
        file.close();
        // READ THE LABEL FILE DATA
        file.open(labelpath, std::ifstream::in | std::ifstream::binary);
        if (file.is_open()) {
            file.read((char*)&magicNum_labels, 4);
            file.read((char*)&itemCount_labels, 4);
            magicNum_labels = swap32(magicNum_labels);
            itemCount_labels = swap32(itemCount_labels);
            if(itemCount_images == itemCount_labels) {
                // read all the labels and store them in theire MNISTchars
                for(MNISTchar& m : tmpdata) {
                    file.read((char*)&m.label, 1);
                }
            }
        }
        file.close();
        return tmpdata;
    }
    
    
public:
    // Print out some digits from the training set
    // (startChar / endChar should be in the range of 0 - 60.000)
    void testPrintout(int startChar, int endChar) const {
        for(int i = startChar; i < endChar; i++) {
            std::cout <<"------------------------------" <<std::endl;
            for (const row& r : trainingData[i].pixelData) {
                for (const int& pixCol : r) {
                    if(pixCol < 50) std::cout <<" ";
                    else if(pixCol <120) std::cout <<"-";
                    else if(pixCol < 200) std::cout <<"+";
                    else if(pixCol <= 255) std::cout <<"#";
                }
                std::cout <<std::endl;
            }
            std::cout <<"\t\tThis is a: " <<trainingData[i].label  <<std::endl;
            std::cout <<"------------------------------" <<std::endl;
        }
    }
    
};

#endif

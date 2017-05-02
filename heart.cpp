/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */

#include <sstream>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

#include <sstream>

template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }

int main(int argc, const char *argv[]) {
   
    if (argc < 3) {
        cout << "usage: " << argv[0] << " <pic_prefix> <dir>"<< endl;
        exit(1);
    }

    VideoCapture cap;
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
    if(!cap.open(0)) {
        cerr << "Error opening webcam\n";
        return 0;
    }

    cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);

    int pic_count = 0;

    for(;;)
    {
        // Resize the images to match the YALE faces
        Size size(320, 243);
        Mat frame;
        Mat resized;

        cap >> frame;
        if( frame.empty() ) break; // end of video stream

        imshow("Webcam", frame);

        int key = waitKey(10);

        if(key != -1) {

            // Save the pictures, that will later be used for training
            resize(frame, resized, size);
            
            printf("Taking Picture \n");
            imwrite(argv[2]+string("/")+argv[1]+NumberToString(pic_count++)+".pgm", resized);
        }
    }

    return 0;
}
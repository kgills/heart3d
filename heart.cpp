#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

CascadeClassifier face_cascade;

String face_cascade_name = "haarcascade_frontalface_alt.xml";

float xy_drift = 150;           // Assuming drift will not be more than this value in either direction

void heart_rate(VideoCapture capture, const char *filename) 
{
    Mat frame;
    vector<Rect> faces;
    Rect prev_face;
    Mat frame_gray;
    Mat face_circle;
    Rect myROI;
    Mat cropped;
    Mat bgr[3]; 
    float prev_dim = 130.0;
    float prev_x = 0.0;
    float prev_y = 0.0;
    float prev_time = 0.0;
    float current_time = 0.0;
    double dt_avg = 0;
    int dt_avg_count = 0;

    int framecount = 0;
    vector<double> r_samples;
    vector<double> g_samples;
    vector<double> b_samples;
    double r_mean = 0;
    double r_sd = 0;
    double r_var = 0;
    double g_mean = 0;
    double g_sd = 0;
    double g_var = 0;
    double b_mean = 0;
    double b_sd = 0;
    double b_var = 0;

    FILE * pFile;
    pFile = fopen(filename,"w");

    while(framecount < 250) {

        // Attempt to read the next frame
        if(!capture.read(frame)) {
            break;
        }

        framecount++;

        // Create the gray frames
        cvtColor( frame, frame_gray, CV_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );

        // Detect the faces, skip this frmae if there is more than one face
        face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, 
            Size(prev_dim*0.9, prev_dim*0.9) , Size(prev_dim*1.1, prev_dim*1.1));

        if(faces.size() == 0) {
            cout << "No faces" << endl;
            faces[0] = prev_face;
        }

        if(faces.size() > 1) {

            int found = 0;

            cout << "More than 1 face" << endl;

            // Attempt to filter by position
            for(int i = 0; i < faces.size(); i++) {
                printf("%d x = %d y = %d width = %d height = %d\n", 
                    i, faces[i].x, faces[i].y, faces[i].width, faces[i].height);

                if(faces[i].x < (prev_x+xy_drift) && (faces[i].x > (prev_x-xy_drift)) && 
                    faces[i].y < (prev_y+xy_drift) && (faces[i].y > (prev_y-xy_drift))) {
                    faces[0] = faces[i];

                    printf("Found the good face %d\n", i);
                    found = 1;
                    break;
                }
            }

            if(!found) {
                cout << "Can't find the face" << endl;
                continue;
            }
        }

        // Save the information for the next iteration
        prev_face=faces[0];
        prev_x = faces[0].x;
        prev_y = faces[0].y;
        prev_dim = faces[0].width;

        // Extract the face
        Rect myROI(faces[0].x+faces[0].width*0.25, faces[0].y, faces[0].width*0.45 , faces[0].height*0.3);
        cropped = frame(myROI);

        // seperate into bgr channels
        split(cropped,bgr);

        // Calculate the average pixel value in each channel
        Scalar b_avg, g_avg, r_avg;
        b_avg = mean(bgr[0]);
        g_avg = mean(bgr[1]);
        r_avg = mean(bgr[2]);

        b_samples.push_back(b_avg[0]);
        g_samples.push_back(g_avg[0]);
        r_samples.push_back(r_avg[0]);

        b_mean += b_avg[0];
        g_mean += g_avg[0];
        r_mean += r_avg[0];

        // Save the frame time
        current_time = capture.get(CV_CAP_PROP_POS_MSEC);
        if(current_time - prev_time > 0) {
            dt_avg += current_time - prev_time;
            dt_avg_count++;
        }

        // printf("%f, %f, %f\n", b_avg[0], g_avg[0], r_avg[0]);

        prev_time = current_time;

        // Display one of the channels
        imshow("video",bgr[1]);
        cvWaitKey(10);
    }

    dt_avg /= dt_avg_count;
    // printf("dt_avg = %f framecount = %d\n", dt_avg, framecount);

    // calculate the mean
    b_mean = b_mean/framecount;
    g_mean = g_mean/framecount;
    r_mean = r_mean/framecount;

    // Calculate the variance and standard deviation
    for(int i = 0; i < framecount; i++) {
        b_var += (b_samples[i] - b_mean) * (b_samples[i] - b_mean);
        g_var += (g_samples[i] - g_mean) * (g_samples[i] - g_mean);
        r_var += (r_samples[i] - r_mean) * (r_samples[i] - r_mean);
    }
    b_var /= framecount;
    g_var /= framecount;
    r_var /= framecount;
    b_sd = sqrt(b_var);
    g_sd = sqrt(g_var);
    r_sd = sqrt(r_var);

    printf("b_mean=%f g_mean=%f r_mean=%f\n", b_mean, g_mean, r_mean);
    printf("b_var=%f g_var=%f r_var=%f\n", b_var, g_var, r_var);
    printf("b_sd=%f g_sd=%f r_sd=%f\n", b_sd, g_sd, r_sd);

    fprintf(pFile, "b,g,r,t\n");

    for(int i = 0; i < framecount; i++) {

        // Normalize the samples
        b_samples[i] = (b_samples[i] - b_mean)/b_sd;
        g_samples[i] = (g_samples[i] - g_mean)/g_sd;
        r_samples[i] = (r_samples[i] - r_mean)/r_sd;

        fprintf(pFile, "%f, %f, %f, %f\n", b_samples[i], g_samples[i], r_samples[i], i*dt_avg);

    }

    fclose(pFile);
}

int main(int argc, const char *argv[]) 
{
   
    if (argc != 3) {
        cout << "usage: " << argv[0] << " <video.avi> <output_file.csv"<< endl;
        return 0;
    }

    // Load the cascades for the face detection
    if( !face_cascade.load( face_cascade_name ) ){ 
        cerr << "Error loading face_cascade" << endl;
        return -1;
    }

    cout << "*** Heart rate from 3D video ***" << endl;

    // Open the video
    VideoCapture capture(argv[1]);
    if (!capture.isOpened() ) {
        cout << "Cannot open the video file. \n";
        return -1;
    }

    double fps = capture.get(CV_CAP_PROP_FPS);
    printf("fps = %f\n", fps);

    heart_rate(capture, argv[2]);

    cout << "Exiting" << endl;
    return 0;
}
#include <iostream>
#include <fstream>
#include <sstream>
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

CascadeClassifier face_cascade;
String face_cascade_name = "haarcascade_frontalface_alt.xml";

float fps = 15.0;
float xy_drift = 150;           // Assuming drift will not be more than this value in either direction

void play_video(VideoCapture capture) 
{
    Mat frame;
    vector<Rect> faces;
    Mat frame_gray;
    Mat face_circle;
    Rect myROI;
    Mat cropped;
    Mat bgr[3]; 
    static float prev_dim = 130.0;
    static float prev_x = 0.0;
    static float prev_y = 0.0;

    while(1) {

        // Attempt to read the next frame
        if(!capture.read(frame)) {
            break;
        }

        // Create the gray frames
        cvtColor( frame, frame_gray, CV_BGR2GRAY );
        equalizeHist( frame_gray, frame_gray );

        // Detect the faces, skip this frmae if there is more than one face
        face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, 
            Size(prev_dim*0.9, prev_dim*0.9) , Size(prev_dim*1.1, prev_dim*1.1));

        if(faces.size() == 0) {
            cout << "No faces" << endl;

            // Relax the size restrictions
            float factor = 0.2;
            float low_val;
            while(1) {
                low_val = (prev_dim*(1-factor));
                if(low_val < 0.0) {
                    low_val = 0.0;
                }
                face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, 
                    Size(low_val, low_val) , Size(prev_dim*(1.0+factor), prev_dim*(1+factor)));
                if(faces.size() > 1) {
                    break;
                }
            }
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

                    found = 1;
                    break;
                }
            }

            if(!found) {
                continue;
            }
        }

        prev_x = faces[0].x;
        prev_y = faces[0].y;
        prev_dim = faces[0].width;

        // Extract the face
        Rect myROI(faces[0].x , faces[0].y-(faces[0].height*0.1), faces[0].width , faces[0].height*1.1);
        cropped = frame(myROI);

        // seperate into bgr channels
        split(cropped,bgr);//split source 
        Scalar b_avg, g_avg, r_avg;
        b_avg = mean(bgr[0]);
        g_avg = mean(bgr[1]);
        r_avg = mean(bgr[2]);
        printf("b_avg=%f g_avg=%f r_avg=%f\n", b_avg[0], g_avg[0], r_avg[0]);

        // Display one of the channels
        imshow("video",bgr[1]);
        cvWaitKey(10);
    }
}

int main(int argc, const char *argv[]) 
{
   
    if (argc != 2) {
        cout << "usage: " << argv[0] << " <video.avi>"<< endl;
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

    // Assuming 15 frames per second
    printf("FPS = %f\n", fps);

    play_video(capture);

    cout << "Exiting" << endl;
    return 0;
}
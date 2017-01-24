#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include "bacterium.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv){
    VideoCapture inVid("D:\\Users\\Neil Zhao\\Documents\\Tencent Files\\1055306580\\FileRecv\\1021\\great.avi");
    Mat inframe, outframe;

    if (!inVid.isOpened()) {
        cerr << "Oops! We cannot open the file" << endl;
        return -1;
    }
    int rate = inVid.get(CV_CAP_PROP_FPS);
    namedWindow("Input", CV_WINDOW_FREERATIO);
    resizeWindow("Input", 960, 813);
    
    inVid >> inframe;
    cvtColor(inframe, outframe, CV_BGR2GRAY);
    tracker track(outframe, 30, 40);
    
    cout << "initialized" << endl;

    while (true && track.isinited) {
        inVid >> inframe;

        if (inframe.empty())
            break;

        cvtColor(inframe, outframe, CV_BGR2GRAY);
        
        if (!track.lost) {
            if (!track.update(outframe))
                if (track.lost)
                    putText(inframe, "Bactetrium lost! Please find a new one!", Point(600, 50),
                        FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255));
                else
                    putText(inframe, "Out of focus!", Point(600, 50),
                        FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 204, 255));
            else
                putText(inframe, "2D fixed", Point(600, 50),
                    FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 255, 0));
            
            putText(inframe, "Contrast: " + to_string(track.contrast), Point(20, 20),
                FONT_HERSHEY_COMPLEX, 0.8, Scalar(255, 255, 255));
            rectangle(inframe, Rect(track.bact.rangel, track.bact.ranger), Scalar(255, 0, 0));
        }
        else {
            putText(inframe, "Bactetrium lost! Please find a new one!", Point(600, 50),
                FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 255));
        }

        setMouseCallback("Input", onMouse, &track);
        imshow("Input", inframe);


        if (waitKey(800 / rate) >= 0)
            break;
    }

    inVid.release();

    return 0;
}
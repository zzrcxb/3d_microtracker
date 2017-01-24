#pragma once

#include <opencv2/opencv.hpp>


typedef struct bacterium {
    cv::Point pos;
    cv::Point rangel, ranger;

    int width;
    double speed;
} bacterium;

enum camera_state {
    stop = 0,
    up = 1,
    down = 2,
    error = 3
};


class tracker {
private:
    cv::Mat inFrame;
    void set_region();

public:
    int initwidth;
    bacterium bact;
    bool isinited;
    bool lost;
    double contrast;
    bool out_focus;

    tracker(cv::Mat inFrame, int width, int radius);
    
    bool init(int radius);
    bool setnewbacterium(bacterium bact);
    bool update(cv::Mat inframe);
};

class camera {
private:
    cv::VideoCapture inVid;

public:
    camera_state cur_state;

    camera(cv::VideoCapture inVid);
    bool autofocus();
};

int tint(double x); // Convert double to int
double get_contrast(cv::Mat inPic);
void onMouse(int event, int x, int y, int flags, void* para);
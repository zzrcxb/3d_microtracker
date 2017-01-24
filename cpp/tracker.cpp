#include "bacterium.h"
#include <iostream>
#include <vector>
#include <Windows.h>

using namespace cv;
using namespace std;


// Note, inFrame is a GRAY_SCALE picture!!!
tracker::tracker(Mat inFrame, int width, int radius) {
    this->inFrame = inFrame.clone();
    bact.width = width;
    initwidth = width;
    lost = false;
    if (!init(radius)) {
        isinited = false;
        cerr << "Cannot initialize! Please set point manually" << endl;
    }
    else
        isinited = true;
}


bool tracker::init(int radius) {
    Mat judge;
    int width = inFrame.cols, height = inFrame.rows;
    int *cnt_row = new int[height], *cnt_column = new int[width];
    double min, max;
    Point center(0, 0);

    cout << width << "\t" << height << endl;
    minMaxLoc(inFrame, &min, &max);

    judge = inFrame >= ((uchar)max - 2);

    judge = judge / 255;
    
    // First stage, get approx point
    for (int i = 0; i < height; i++)
        cnt_row[i] = 0;
    for (int i = 0; i < width; i++)
        cnt_column[i] = 0;

    for (int i = 0; i < height; i++) {
        uchar* ptr = judge.ptr<uchar>(i);
        for (int j = 0; j < width; j++) {
            if (ptr[j] == 1) {
                cnt_row[i]++;
                cnt_column[j]++;
            }
        }
    }

    for (int i = 0, max = 0; i < height; i++) {
        if (cnt_row[i] > max) {
            max = cnt_row[i];
            center.y = i;
        }
    }
    
    for (int j = 0, max = 0; j < width; j++) {
        if (cnt_column[j] > max) {
            max = cnt_column[j];
            center.x = j;
        }
    }

    cout << "First time search, " << center << endl;
    //Second
    int cnt = 0;
    double sumx = 0, sumy = 0;
    for (int i = 0; i < judge.rows; i++) {
        uchar* ptr_p = judge.ptr<uchar>(i);
        for (int j = 0; j < judge.cols; j++) {
            if ((i - center.y) * (i - center.y) + (j - center.x) * (j - center.x)
                < radius * radius && ptr_p[j] == 1) {
                cnt++;
                sumx += j;
                sumy += i;
            }
        }
    }

    int resx = tint(sumx / (double)cnt);
    int resy = tint(sumy / (double)cnt);

    if ((resx - center.x) * (resx - center.x) +
        (resy - center.y) * (resy - center.y) > radius * radius / 4) {
        return true;
    }
    else {
        bact.pos.x = resx;
        bact.pos.y = resy;
    }

    cout << "Find bacterium at " << bact.pos << endl;

    set_region();

    delete[] cnt_column;
    delete[] cnt_row;

    return true;
}


bool tracker::setnewbacterium(bacterium bact) {
    this->bact = bact;
    set_region();
    return true;
}


bool tracker::update(Mat inframe) {
    double min, max;
    double min_global, max_global;
    double global_contrast;
    const int scale = 20;
    
    Rect focus_region(bact.rangel, bact.ranger);
    Mat focus_window = inframe(focus_region);

    // Get local region properties
    minMaxLoc(focus_window, &min, &max);

    double sumx = 0, sumy = 0;
    int cnt = 0;
    for (int i = 0; i < focus_window.rows; i++) {
        uchar* ptr = focus_window.ptr<uchar>(i);
        for (int j = 0; j < focus_window.cols; j++) {
            if (ptr[j] >= (int)max - scale) {
                sumx += j;
                sumy += i;
                cnt++;
            }
        }
    }

    bact.pos.x = tint(sumx / cnt) + bact.rangel.x;
    bact.pos.y = tint(sumy / cnt) + bact.rangel.y;

    {
        uchar* ptr = focus_window.ptr<uchar>(tint(sumy / cnt));
        int radius = bact.width, low = 0, high = 0;
        bool islowset = false;

        for (int j = 0; j < focus_window.cols; j++) {
            if (ptr[j] >= (int)max - scale) {
                if (!islowset) {
                    low = j;
                    islowset = true;
                }
                else
                    high = j;
            }
        }
        radius = high - low;
        if (radius > initwidth - 5)
            bact.width = radius + 5;
        else
            bact.width = initwidth;
    }

    if (bact.pos.x < bact.width || bact.pos.x > inframe.cols - bact.width ||
        bact.pos.y < bact.width || bact.pos.y > inframe.rows - bact.width) {
        lost = true;
        return false;
    }

    set_region();

    contrast = get_contrast(focus_window);

    // Get global properties
    // global_contrast = get_contrast(inframe);
    minMaxLoc(inframe, &min_global, &max_global);

    if (contrast <= 150 || max_global > 1.2 * max) {
        out_focus = true;
        return false;
    }
    else {
        out_focus = false;
        return true;
    }
}


void tracker::set_region() {

    // Set up range
    if (bact.pos.x - bact.width < 0)
        bact.rangel.x = 0;
    else
        bact.rangel.x = bact.pos.x - bact.width;

    if (bact.pos.y - bact.width < 0)
        bact.rangel.y = 0;
    else
        bact.rangel.y = bact.pos.y - bact.width;

    if (bact.pos.x + bact.width >= inFrame.cols)
        bact.ranger.x = inFrame.cols - 1;
    else
        bact.ranger.x = bact.pos.x + bact.width;

    if (bact.pos.y + bact.width >= inFrame.rows)
        bact.ranger.y = inFrame.rows - 1;
    else
        bact.ranger.y = bact.pos.y + bact.width;

}


double get_contrast(cv::Mat frame) {
    int width = frame.cols - 2;
    int height = frame.rows - 2;
    Mat inframe = frame.clone();

    Rect center_r(1, 1, width, height);
    Rect up_r(1, 0, width, height);
    Rect down_r(1, 2, width, height);
    Rect left_r(0, 1, width, height);
    Rect right_r(2, 1, width, height);

    Mat up;
    Mat down;
    Mat left;
    Mat right;
    Mat center;

    inframe(up_r).clone().convertTo(up, CV_32S);
    inframe(down_r).clone().convertTo(down, CV_32S);
    inframe(left_r).clone().convertTo(left, CV_32S);
    inframe(right_r).clone().convertTo(right, CV_32S);
    inframe(center_r).clone().convertTo(center, CV_32S);

    multiply((up - center), (up - center), up);
    multiply((down - center), (down - center), down);
    multiply((left - center), (left - center), left);
    multiply((right - center), (right - center), right);

    return (double)(sum(up + down + left + right)[0]) / (width * height * 1.0);
}

void onMouse(int event, int x, int y, int flags, void* para) {
    tracker* track = (tracker*)para;

    if (event == EVENT_LBUTTONDOWN) {
        Sleep(20);
        bacterium bact;
        bact.pos.x = x;
        bact.pos.y = y;
        bact.width = track->initwidth;

        track->setnewbacterium(bact);
        track->lost = false;
    }
}


int tint(double x) {
    if (x - (int)x >= 0.5)
        return (int)x + 1;
    else
        return (int)x;
}

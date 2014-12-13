#ifndef VISIONTEST_H
#define VISIONTEST_H

#include <iostream>
#include <vector>
#include <thread>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "ControlsWindow.h"
#include "Slider.h"
#include "ModBlobDetector.h"
#include "draw.h"
#include "miscutils.h"
#include "preferences.h"
#include "oscheck.h"

#ifdef NIX
#include <libv4l2.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

using namespace std;
using namespace cv;

Mat applyMask(Mat input, Mat mask);
int process(VideoCapture& capture);
Mat thresholdImage(ControlsWindow* controlsWindow, Mat image);
Mat erodeDilate(Mat src, ControlsWindow* ctrlWin);
rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value);
void rotated_rect(Mat im, const RotatedRect & rot_rect, CvScalar color);
void loadCameraSettings(char* filename);

#endif

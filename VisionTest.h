#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <ctime>
#include "ControlsWindow.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "Slider.h"
#include "ModBlobDetector.h"
#include "draw.h"

//if we're not on Windows
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__))
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
void saveSettings(ControlsWindow* cwin, char* filename);
void loadSettings(ControlsWindow* cwin, char* filename);
rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value);
std::string toString(int i);
std::string toString(double i);
bool fileExists(const std::string& name);
void rotated_rect(Mat im, const RotatedRect & rot_rect, CvScalar color);
void loadCameraSettings(char* filename);

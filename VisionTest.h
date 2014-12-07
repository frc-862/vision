#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include "ControlsWindow.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "Slider.h"
#include "ModBlobDetector.h"

using namespace cv;

Mat applyMask(Mat input, Mat mask);
int process(VideoCapture& capture);
Mat thresholdImage(ControlsWindow* controlsWindow, Mat image);
Mat erodeDilate(Mat src, ControlsWindow* ctrlWin);
void saveSettings(ControlsWindow* cwin, char* filename);
void loadSettings(ControlsWindow* cwin, char* filename);
rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value);
std::string toString(int i);
bool fileExists(const std::string& name);

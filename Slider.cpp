#include "Slider.h"
#include "opencv2/highgui/highgui.hpp"

Slider::Slider(Window* parentWindow, std::string title, int max)
{
	maxValue = max;
	name = title;
	parent = parentWindow;
	cv::createTrackbar(title, parentWindow->getName(), &value, maxValue, NULL, NULL);
}

Slider::~Slider() {}

void Slider::setValue(int newValue) {
	value = newValue;
	cv::setTrackbarPos(name, parent->getName(), newValue);
}

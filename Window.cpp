#include "Window.h"
#include "opencv2/highgui/highgui.hpp"
#include "Slider.h"

Window::Window(std::string title)
{
	name = title;
	cv::namedWindow(title, CV_WINDOW_NORMAL);
}


Window::~Window()
{
	cv::destroyWindow(name);
	for (WindowElement* element : elements) {
		delete(element);
	}
}

Slider* Window::addSlider(std::string sliderName, int maxValue) {
    Slider* sl = new Slider(this, sliderName, maxValue);
    elements.push_back(sl);
    return sl;
}

#pragma once
#include <string>
#include <vector>
#include "WindowElement.h"

class Slider;

class Window
{
public:
	Window(std::string);
	~Window();
	std::string getName() { return name; }
	Slider* addSlider(std::string name, int maxValue);
private:
	std::string name;
	std::vector<WindowElement*> elements;
};


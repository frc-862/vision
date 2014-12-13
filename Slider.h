#ifndef SLIDER_H
#define SLIDER_H
#pragma once
#include "WindowElement.h"
#include "Window.h"

class Slider :
	public WindowElement
{
public:
	Slider(Window* parentWindow, std::string title, int maxValue);
	~Slider();
	int getValue() { return value; }
	void setValue(int newValue);
private:
	int value = 0;
	int maxValue;
	std::string name;
	Window* parent;
};

#endif

#ifndef CONTROLSWINDOW_H
#define CONTROLSWINDOW_H
#pragma once
#include "Window.h"
class ControlsWindow :
	public Window
{
public:
	ControlsWindow();
	~ControlsWindow();

	/*
	 * For the following getters, return the values to be used, not necessarily the slider values
	 */
	int getBrightness();
	float getContrast();
	float getBlurDeviation();
	int getMinHue();
	int getMaxHue();
	int getMinSat();
	int getMaxSat();
	int getMinVal();
	int getMaxVal();
	int getDilation();
	int getErosion();
    bool getInvert();

	Slider* brightnessSlider;
	Slider* contrastSlider;
	Slider* blurSlider;
	Slider* minHueSlider;
	Slider* maxHueSlider;
	Slider* minSatSlider;
	Slider* maxSatSlider;
	Slider* minValSlider;
	Slider* maxValSlider;
	Slider* dilateSlider;
	Slider* erodeSlider;
    Slider* invertSlider;
private:

};
#endif

#include "ControlsWindow.h"
#include "Slider.h"

const int RELATIVE_MAX_CONTRAST = 3;
const int CONTRAST_SLIDER_MAX = 100;

ControlsWindow::ControlsWindow()
	: Window("Controls")
{
	brightnessSlider = addSlider("Brightness", 255);
	contrastSlider = addSlider("Contrast", CONTRAST_SLIDER_MAX);
	blurSlider = addSlider("Blur", 100);

	contrastSlider->setValue((1.0 / RELATIVE_MAX_CONTRAST)*CONTRAST_SLIDER_MAX);

	minHueSlider = addSlider("Min Hue", 256);
	maxHueSlider = addSlider("Max Hue", 256);

	minValSlider = addSlider("Min Value", 256);
	maxValSlider = addSlider("Max Value", 256);

	minSatSlider = addSlider("Min Saturation", 256);
	maxSatSlider = addSlider("Max Saturation", 256);

	dilateSlider = addSlider("Dilation", 10);
	erodeSlider = addSlider("Erosion", 10);

    invertSlider = addSlider("Invert", 1);

	maxHueSlider->setValue(256);
	maxValSlider->setValue(256);
	maxSatSlider->setValue(256);
}

ControlsWindow::~ControlsWindow()
{
}

int ControlsWindow::getBrightness() {
	return brightnessSlider->getValue();
}

float ControlsWindow::getContrast() {
	return (contrastSlider->getValue() / (double)CONTRAST_SLIDER_MAX)*RELATIVE_MAX_CONTRAST;
}

float ControlsWindow::getBlurDeviation() {
	return (blurSlider->getValue()/100.0)*2;
}

int ControlsWindow::getMinHue() {
	return minHueSlider->getValue()-1;
}

int ControlsWindow::getMaxHue() {
	return maxHueSlider->getValue()-1;
}

int ControlsWindow::getMinSat() {
	return minSatSlider->getValue()-1;
}

int ControlsWindow::getMaxSat() {
	return maxSatSlider->getValue()-1;
}

int ControlsWindow::getMinVal() {
	return minValSlider->getValue()-1;
}

int ControlsWindow::getMaxVal() {
	return maxValSlider->getValue()-1;
}

int ControlsWindow::getDilation() {
    return dilateSlider->getValue();
}

int ControlsWindow::getErosion() {
    return erodeSlider->getValue();
}

bool ControlsWindow::getInvert() {
    return invertSlider->getValue() == 1;
}

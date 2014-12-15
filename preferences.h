#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <ostream>
#include "miscutils.h"
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "ControlsWindow.h"
#include "Slider.h"
#include "oscheck.h"

#ifdef NIX
#include <linux/v4l2-controls.h>
#endif

struct CameraSettings {
    float focalLength = -1;
#ifdef NIX
    int exposureMode = V4L2_EXPOSURE_AUTO;
#else
    int exposureMode = -1;
#endif
    float exposureTime = -1;
    float gain = -1;
};

CameraSettings loadCameraSettings(char* filename);

void saveSettings(ControlsWindow* cwin, char* filename);
void loadSettings(ControlsWindow* cwin, char* filename);

rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value);

#endif // PREFERENCES_H

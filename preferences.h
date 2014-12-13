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
void loadCameraSettings(char* filename, double& focalLength, int& exposureMode);
#else
void loadCameraSettings(char* filename, double& focalLength);
#endif

void saveSettings(ControlsWindow* cwin, char* filename);
void loadSettings(ControlsWindow* cwin, char* filename);

rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value);

#endif // PREFERENCES_H

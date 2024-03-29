#include "preferences.h"

CameraSettings loadCameraSettings(char* filename) {
    CameraSettings camSettings;
    rapidxml::xml_document<> doc;
    rapidxml::file<> file(filename);

    doc.parse<0>(file.data());

    rapidxml::xml_node<>* focalLengthNode = doc.first_node("FocalLength");
    if(focalLengthNode != nullptr) {
        camSettings.focalLength = atof(focalLengthNode->value());
    }

#ifdef NIX
    rapidxml::xml_node<>* exposureModeNode = doc.first_node("ExposureMode");
    if(exposureModeNode != nullptr) {
        camSettings.exposureMode = atoi(exposureModeNode->value());
    }
#endif

    rapidxml::xml_node<>* expTimeNode = doc.first_node("ExposureTime");
    if(expTimeNode != nullptr) {
        camSettings.exposureTime = atof(expTimeNode->value());
    }

    rapidxml::xml_node<>* gainNode = doc.first_node("Gain");
    if(gainNode != nullptr) {
        camSettings.gain = atof(gainNode->value());
    }

    doc.clear();

    return camSettings;
}

ObjectInfo loadObjectInfo(char* filename) {
    ObjectInfo objInfo;
    rapidxml::xml_document<> doc;
    rapidxml::file<> file(filename);

    doc.parse<0>(file.data());

    rapidxml::xml_node<>* widthNode = doc.first_node("Width");
    if(widthNode != nullptr) {
        float width = atof(widthNode->value());
        if(width < 0) {
            std::cerr << "Width in object config is negative!!! THIS IS A PROBLEM!" << std::endl;
        } else {
            objInfo.width = width;
        }
    }

    rapidxml::xml_node<>* heightNode = doc.first_node("Height");
    if(widthNode != nullptr) {
        float height = atof(heightNode->value());
        if(height < 0) {
            std::cerr << "Height in object config is negative!!! THIS IS A PROBLEM!" << std::endl;
        } else {
            objInfo.height = height;
        }
    }

    float longestSide = std::max(objInfo.width, objInfo.height);
    float shortestSide = std::min(objInfo.width, objInfo.height);
    if(longestSide != 0 && shortestSide != 0) {
        objInfo.aspectRatio = longestSide/shortestSide;
    }

    return objInfo;
}

void loadSettings(ControlsWindow* cwin, char* fileName) {
    rapidxml::xml_document<> doc;
    rapidxml::file<> file(fileName);

    doc.parse<0>(file.data());

    rapidxml::xml_node<>* valuesNode = doc.first_node("Values");

    rapidxml::xml_node<>* brightnessNode = valuesNode->first_node("Brightness");
    rapidxml::xml_node<>* contrastNode = valuesNode->first_node("Contrast");
    rapidxml::xml_node<>* blurNode = valuesNode->first_node("Blur");
    rapidxml::xml_node<>* minHueNode = valuesNode->first_node("MinHue");
    rapidxml::xml_node<>* maxHueNode = valuesNode->first_node("MaxHue");
    rapidxml::xml_node<>* minSatNode = valuesNode->first_node("MinSat");
    rapidxml::xml_node<>* maxSatNode = valuesNode->first_node("MaxSat");
    rapidxml::xml_node<>* minValNode = valuesNode->first_node("MinVal");
    rapidxml::xml_node<>* maxValNode = valuesNode->first_node("MaxVal");
    rapidxml::xml_node<>* dilationNode = valuesNode->first_node("Erosion");
    rapidxml::xml_node<>* erosionNode = valuesNode->first_node("Dilation");

    cwin->brightnessSlider->setValue(atoi(brightnessNode->value()));
    cwin->contrastSlider->setValue(atoi(contrastNode->value()));
    cwin->blurSlider->setValue(atoi(blurNode->value()));
    cwin->minHueSlider->setValue(atoi(minHueNode->value()));
    cwin->maxHueSlider->setValue(atoi(maxHueNode->value()));
    cwin->minSatSlider->setValue(atoi(minSatNode->value()));
    cwin->maxSatSlider->setValue(atoi(maxSatNode->value()));
    cwin->minValSlider->setValue(atoi(minValNode->value()));
    cwin->maxValSlider->setValue(atoi(maxValNode->value()));
    cwin->erodeSlider->setValue(atoi(erosionNode->value()));
    cwin->dilateSlider->setValue(atoi(dilationNode->value()));

    doc.clear();
}

void saveSettings(ControlsWindow* cwin, char* filename) {
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<>* valuesNode = doc.allocate_node(rapidxml::node_element, "Values");

    rapidxml::xml_node<>* brightnessNode = createValNode(&doc, (char*)"Brightness", cwin->brightnessSlider->getValue());
    valuesNode->append_node(brightnessNode);

    rapidxml::xml_node<>* contrastNode = createValNode(&doc, (char*)"Contrast", cwin->contrastSlider->getValue());
    valuesNode->append_node(contrastNode);

    rapidxml::xml_node<>* blurNode = createValNode(&doc, (char*)"Blur", cwin->blurSlider->getValue());
    valuesNode->append_node(blurNode);

    rapidxml::xml_node<>* minHueNode = createValNode(&doc, (char*)"MinHue", cwin->minHueSlider->getValue());
    valuesNode->append_node(minHueNode);
    rapidxml::xml_node<>* maxHueNode = createValNode(&doc, (char*)"MaxHue", cwin->maxHueSlider->getValue());
    valuesNode->append_node(maxHueNode);

    rapidxml::xml_node<>* minSatNode = createValNode(&doc, (char*)"MinSat", cwin->minSatSlider->getValue());
    valuesNode->append_node(minSatNode);
    rapidxml::xml_node<>* maxSatNode = createValNode(&doc, (char*)"MaxSat", cwin->maxSatSlider->getValue());
    valuesNode->append_node(maxSatNode);

    rapidxml::xml_node<>* minValNode = createValNode(&doc, (char*)"MinVal", cwin->minValSlider->getValue());
    valuesNode->append_node(minValNode);
    rapidxml::xml_node<>* maxValNode = createValNode(&doc, (char*)"MaxVal", cwin->maxValSlider->getValue());
    valuesNode->append_node(maxValNode);

    rapidxml::xml_node<>* erosionNode = createValNode(&doc, (char*)"Erosion", cwin->erodeSlider->getValue());
    valuesNode->append_node(erosionNode);
    rapidxml::xml_node<>* dilationNode = createValNode(&doc, (char*)"Dilation", cwin->dilateSlider->getValue());
    valuesNode->append_node(dilationNode);

    doc.append_node(valuesNode);

    std::ofstream fileout;
    fileout.open(filename);
    fileout << doc;

    fileout.close();
    doc.clear();
}

rapidxml::xml_node<>* createValNode(rapidxml::xml_document<>* doc, char* name, int value) {
    char* valueStr = doc->allocate_string(toString(value).c_str());
    rapidxml::xml_node<>* valueNode = doc->allocate_node(rapidxml::node_element, name, valueStr);

    return valueNode;
}

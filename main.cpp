#include "VisionTest.h"

using namespace std;

const string VIDEO_WINDOW_NAME = "Video";
const string ERODE_PREVIEW_WIN_NAME = "Erosion/Dilation Preview";
const int GAUSSIAN_KERNEL = 7;

char* preferenceFileName = (char*)"default.xml";

int main(int argc, char** args) {
    if(argc > 1) {
        preferenceFileName = args[1];
    }

	VideoCapture capture(0); //0 is the default camera
	if (!capture.isOpened()) {
		cerr << "Failed to open camera!" << endl;
		return 1;
	}
	return process(capture);
}

Mat applyMask(Mat input, Mat mask) {
	Mat newImage;
	input.copyTo(newImage, mask);
	return newImage;
}

int process(VideoCapture& capture) {
	cout << "Press q or escape to quit!" << endl;

	namedWindow(VIDEO_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
	namedWindow(ERODE_PREVIEW_WIN_NAME, CV_WINDOW_NORMAL);
	resizeWindow(ERODE_PREVIEW_WIN_NAME, 320, 240);
	ControlsWindow* controlsWindow = new ControlsWindow();

    if(fileExists(preferenceFileName)) {
        loadSettings(controlsWindow, (char*)preferenceFileName);
    }

	Mat frame;
	while (true) {
		capture >> frame;
		if (frame.empty())
			break;

		if (controlsWindow->getBlurDeviation() > 0) {
			GaussianBlur(frame, frame, Size(GAUSSIAN_KERNEL, GAUSSIAN_KERNEL), controlsWindow->getBlurDeviation());
		}

		//Apply brightness and contrast
		frame.convertTo(frame, -1, controlsWindow->getContrast(), controlsWindow->getBrightness());

		Mat maskedImage = thresholdImage(controlsWindow, frame);

		Mat erodedImage = erodeDilate(maskedImage, controlsWindow);

		Mat erodedImageBinary;

		threshold(erodedImage, erodedImageBinary, 0, 255, CV_THRESH_BINARY);

		cvtColor(erodedImageBinary, erodedImageBinary, COLOR_BGR2GRAY);

		cv::SimpleBlobDetector::Params params;
		params.minDistBetweenBlobs = 50.0f;
		params.filterByInertia = false;
		params.filterByConvexity = false;
        params.filterByColor = true;
		params.filterByCircularity = false;
		params.filterByArea = true;
		params.minArea = 1000.0f;
		params.maxArea = 100000.0f;
		params.blobColor = 255;

		vector<KeyPoint> centers;
		vector<vector<Point>> contours;
		ModBlobDetector* blobDetector = new ModBlobDetector(params);

        vector<vector<Point>> contourHulls;
        vector<Point2f*> contourRects;
		blobDetector->findBlobs(erodedImageBinary, erodedImageBinary, centers, contours);
        for(vector<Point> ctpts : contours) {
            vector<Point> hull;
            convexHull(ctpts, hull);
            contourHulls.push_back(hull);
            Point2f rectPoints[4];
            minAreaRect(ctpts).points(rectPoints);
            contourRects.push_back(rectPoints);
        }
		cvtColor(erodedImageBinary, erodedImageBinary, COLOR_GRAY2BGR); //So we can draw colors on the mask image
        drawContours( erodedImageBinary, contourHulls, -1, Scalar(255, 128,0), 2);
        drawContours( erodedImageBinary, contours, -1, Scalar(128,255,128), 2);
        for(Point2f* pts : contourRects) {
            for( int j = 0; j < 4; j++ )
                      line( erodedImageBinary, pts[j], pts[(j+1)%4], Scalar(0,0,255), 1, 8 );
        }
		int ptnum;
		for(KeyPoint pt : centers) {
			Scalar color(255, 0, 255);
			circle(erodedImageBinary, pt.pt, 5
					, color, -1 /*filled*/);
			circle(erodedImageBinary, pt.pt, pt.size, color, 1);
			ptnum++;
		}
        delete blobDetector;


		imshow(ERODE_PREVIEW_WIN_NAME, erodedImageBinary);

		imshow(VIDEO_WINDOW_NAME, erodedImage);

		char key = (char)waitKey(1); //Delay 1ms
		switch (key) {
		case 'q':
		case 'Q':
		case 27: //escape
            saveSettings(controlsWindow, (char*)preferenceFileName);
			return 0;
		default:
			break;
		}
	}

    saveSettings(controlsWindow, (char*)preferenceFileName);
    delete(controlsWindow);
	return 0;
}

Mat thresholdImage(ControlsWindow* controlsWindow, Mat image) {
	Mat hsvFrame;
	cvtColor(image, hsvFrame, CV_BGR2HSV);

	vector<Mat> channels;
	split(hsvFrame, channels);
	Mat hue = channels.at(0);
	Mat sat = channels.at(1);
	Mat val = channels.at(2);

	int minHue = min(controlsWindow->getMinHue(), controlsWindow->getMaxHue());
	int maxHue = max(controlsWindow->getMinHue(), controlsWindow->getMaxHue());
	int minSat = min(controlsWindow->getMinSat(), controlsWindow->getMaxSat());
	int maxSat = max(controlsWindow->getMinSat(), controlsWindow->getMaxSat());
	int minVal = min(controlsWindow->getMinVal(), controlsWindow->getMaxVal());
	int maxVal = max(controlsWindow->getMinVal(), controlsWindow->getMaxVal());

	Mat hueMinMask;
	Mat hueMaxMask;
	threshold(hue, hueMinMask, minHue, 255, CV_THRESH_BINARY);
	threshold(hue, hueMaxMask, maxHue, 255, CV_THRESH_BINARY_INV);

	Mat satMinMask;
	Mat satMaxMask;
	threshold(sat, satMinMask, minSat, 255, CV_THRESH_BINARY);
	threshold(sat, satMaxMask, maxSat, 255, CV_THRESH_BINARY_INV);

	Mat valMinMask;
	Mat valMaxMask;
	threshold(val, valMinMask, minVal, 255, CV_THRESH_BINARY);
	threshold(val, valMaxMask, maxVal, 255, CV_THRESH_BINARY_INV);

	return applyMask(applyMask(applyMask(applyMask(applyMask(applyMask(image,
		hueMaxMask), hueMinMask), valMaxMask), valMinMask), satMaxMask), satMinMask);
}

Mat erodeDilate(Mat src, ControlsWindow* ctrlWin) {
    Mat output;

    Mat dilateElement = getStructuringElement( MORPH_RECT,
                                       Size( 2*ctrlWin->getDilation() + 1, 2*ctrlWin->getDilation()+1 ),
                                       Point( ctrlWin->getDilation(), ctrlWin->getDilation() ) );
    dilate(src, output, dilateElement);

    Mat erodeElement = getStructuringElement( MORPH_RECT,
                                       Size( 2*ctrlWin->getErosion() + 1, 2*ctrlWin->getErosion()+1 ),
                                       Point( ctrlWin->getErosion(), ctrlWin->getErosion() ) );
    erode(output, output, erodeElement);

    return output;
}

void loadSettings(ControlsWindow* cwin, char* filename) {
    rapidxml::xml_document<> doc;
    rapidxml::file<> file(filename);

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

    ofstream fileout;
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

string toString(int i) {
	ostringstream convert;
	convert << i;
	return convert.str();
}

bool fileExists(const std::string& name) {
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

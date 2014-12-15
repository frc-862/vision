#include "VisionTest.h"

//#define DEBUG_BLOBS

const int CAMERA_ID = 0;
const string VIDEO_WINDOW_NAME = "Video";
const string ERODE_PREVIEW_WIN_NAME = "Mask Preview";
const string MASKED_PREVIEW_NAME = "Masked Color Preview";
const int GAUSSIAN_KERNEL = 7;

ObjectInfo objInfo;
CameraSettings camSettings;

char* preferenceFileName = (char*)"default.xml";

//if we're not on Windows
#ifdef NIX
void disableAutoExposure() {
    string vidDevice = "/dev/video";
    vidDevice.append(toString(CAMERA_ID));
    int descriptor = v4l2_open(vidDevice.c_str(), O_RDWR);

    v4l2_control c;
    c.id = V4L2_CID_EXPOSURE_AUTO;
    c.value = camSettings.exposureMode;
    if(v4l2_ioctl(descriptor, VIDIOC_S_CTRL, &c) == 0)
        cout << "Disabled auto exposure" << endl;

    c.id = V4L2_CID_EXPOSURE_AUTO_PRIORITY;
    c.value = 0;
    if(v4l2_ioctl(descriptor, VIDIOC_S_CTRL, &c) == 0)
        cout << "Disabled auto priority" << endl;

    v4l2_close(descriptor);
}
#endif

int main(int argc, char** args) {
    if(fileExists("camera.xml")) {
        camSettings = loadCameraSettings((char*)"camera.xml");
    }
    if(fileExists("object.xml")) {
        objInfo = loadObjectInfo((char*)"object.xml");
    }

#ifdef NIX
    disableAutoExposure();
#endif

    if(argc > 1) {
        preferenceFileName = args[1];
    }

    VideoCapture capture(CAMERA_ID);
    if (!capture.isOpened()) {
        cerr << "Failed to open camera!" << endl;
        return 1;
    }

    //Set camera properties
    if(camSettings.gain != -1) {
        capture.set(CV_CAP_PROP_GAIN, camSettings.gain);
    }
    if(camSettings.exposureTime != -1) {
        capture.set(CV_CAP_PROP_EXPOSURE, camSettings.exposureTime);
    }

    return process(capture);
}

Mat applyMask(Mat input, Mat mask) {
    Mat newImage;
    input.copyTo(newImage, mask);
    return newImage;
}

float distance(Point2f p1, Point2f p2) {
    int xDiff = abs(p1.x - p2.x);
    int yDiff = abs(p1.y - p2.y);
    return sqrt(pow(xDiff, 2) + pow(yDiff, 2));
}

int process(VideoCapture& capture) {
    long captureTime;
    cout << "Press q or escape to quit!" << endl;

    CvFont infoFont;
    cvInitFont(&infoFont, CV_FONT_HERSHEY_SIMPLEX, 1, 1);

    int framerate = 30; //Target number of frames to render per second

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
        captureTime = (int)(getTickCount()/getTickFrequency())*1000;
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

        cvtColor(erodedImage, erodedImageBinary, COLOR_BGR2GRAY);
        threshold(erodedImageBinary, erodedImageBinary, 0, 255, CV_THRESH_BINARY);

        if(controlsWindow->getInvert()) {
            erodedImageBinary = 255 - erodedImageBinary;
        }

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
        vector<RotatedRect> contourRects;
        blobDetector->findBlobs(erodedImageBinary, erodedImageBinary, centers, contours);
        for(vector<Point> ctpts : contours) {
            vector<Point> hull;
            convexHull(ctpts, hull);
            contourHulls.push_back(hull);
            contourRects.push_back(minAreaRect(hull));
        }
#ifdef DEBUG_BLOBS
        drawContours(frame, contours, -1, Scalar(128,255,128), 2, CV_AA);
        drawContours(frame, contourHulls, -1, Scalar(255, 128,0), 2, CV_AA);
        int ptnum;
        for(KeyPoint pt : centers) {
            Scalar color(255, 0, 255);
            circle(frame, pt.pt, 5
                   , color, -1 /*filled*/, CV_AA);
            circle(frame, pt.pt, pt.size, color, 1, CV_AA);
            ptnum++;
        }
#endif
        for(RotatedRect rr : contourRects) {
            Point2f points[4];
            rr.points(points);
            float side1 = distance(points[0], points[1]);
            float side2 = distance(points[1], points[2]);

            float shortestSide = min(side1, side2);
            float longestSide = max(side1, side2);
            float aspectRatio = longestSide/shortestSide;
            int b = 0;
            bool isTape = objInfo.aspectRatio == 0 ? false :
                          abs(objInfo.aspectRatio - aspectRatio) < 0.2*objInfo.aspectRatio;
            /*
             * TODO
             * Make a list of possible tape candidates
             * Use tape candidate with smallest difference in ratio to the real ratio as the tape
             */
            if(isTape) {
                b = 255;
                string widthText = "Width (px): ";
                widthText.append(toString(longestSide));
                string heightText = "Height (px): ";
                heightText.append(toString(shortestSide));
                string rotText = "Rotation (deg): ";
                rotText.append(toString((int)rr.angle));
                string distText;
                if(camSettings.focalLength == -1) {
                    distText = "Focal length not defined";
                } else {
                    float dist = objInfo.width * camSettings.focalLength / longestSide;
                    distText = "Distance (cm): ";
                    distText.append(toString(dist));
                }
                putText(frame, widthText, Point(0, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 255));
                putText(frame, heightText, Point(0, 40), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 255));
                putText(frame, rotText, Point(0, 60), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 255));
                putText(frame, distText, Point(0, 80), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 255));
            }

            rotated_rect(frame, rr, Scalar(b, 0, 255));
            if(isTape)break;
        }
        if(objInfo.aspectRatio == 0) {
            putText(frame, "Invalid object info (object.xml)", Point(0, 20), CV_FONT_HERSHEY_SIMPLEX, 0.5f, Scalar(0, 255, 255));
        }
        delete blobDetector;

        imshow(ERODE_PREVIEW_WIN_NAME, erodedImageBinary);

        imshow(VIDEO_WINDOW_NAME, frame);

        int waitTime = max((int)(((1.0/framerate)*1000)
                           - ((int)(getTickCount()/getTickFrequency())*1000 - captureTime))
                           , 1);
        char key = (char)waitKey(waitTime); //Delay 1ms
        switch (key) {
        case 'q':
        case 'Q':
        case 27: //escape
            saveSettings(controlsWindow, (char*)preferenceFileName);
            return 0;
        default:
            break;
        }
        std::this_thread::yield();
    }

    saveSettings(controlsWindow, (char*)preferenceFileName);
    delete(controlsWindow);

    destroyAllWindows();
    return 0;
}

Mat thresholdImage(ControlsWindow* controlsWindow, Mat image) {
    Mat hsvFrame;
    cvtColor(image, hsvFrame, CV_BGR2HSV); //profiler shows this is SLOW

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

    Mat mask;
    inRange(hsvFrame, Scalar(minHue, minSat, minVal), Scalar(maxHue, maxSat, maxVal), mask);

    Mat newMask;

    threshold(mask, newMask, 0, 255, CV_THRESH_BINARY);

    return applyMask(image, newMask);
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

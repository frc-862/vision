#ifndef MODBLOBDETECTOR_H
#define MODBLOBDETECTOR_H

#include "opencv2/features2d/features2d.hpp"

using namespace cv;

class ModBlobDetector : public SimpleBlobDetector {
    public:
        ModBlobDetector(SimpleBlobDetector::Params params) : SimpleBlobDetector(params){}
        void findBlobs(const cv::Mat &image, const cv::Mat &binaryImage, vector<KeyPoint> &centers, vector<vector<Point>> &contours) const;
};

void ModBlobDetector::findBlobs(const cv::Mat &image, const cv::Mat &binaryImage, vector<KeyPoint> &centers, vector<vector<Point>> &contours) const
{
    (void)image;
    centers.clear();

    Mat tmpBinaryImage = binaryImage.clone();
    findContours(tmpBinaryImage, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

#ifdef DEBUG_BLOB_DETECTOR
    //  Mat keypointsImage;
    //  cvtColor( binaryImage, keypointsImage, CV_GRAY2RGB );
    //
    //  Mat contoursImage;
    //  cvtColor( binaryImage, contoursImage, CV_GRAY2RGB );
    //  drawContours( contoursImage, contours, -1, Scalar(0,255,0) );
    //  imshow("contours", contoursImage );
#endif

    for (size_t contourIdx = 0; contourIdx < contours.size(); contourIdx++)
    {
        KeyPoint center;
        Moments moms = moments(Mat(contours[contourIdx]));
        if (params.filterByArea)
        {
            double area = moms.m00;
            if (area < params.minArea || area >= params.maxArea)
                continue;
        }

        if (params.filterByCircularity)
        {
            double area = moms.m00;
            double perimeter = arcLength(Mat(contours[contourIdx]), true);
            double ratio = 4 * CV_PI * area / (perimeter * perimeter);
            if (ratio < params.minCircularity || ratio >= params.maxCircularity)
                continue;
        }

        if (params.filterByInertia)
        {
            double denominator = sqrt(pow(2 * moms.mu11, 2) + pow(moms.mu20 - moms.mu02, 2));
            const double eps = 1e-2;
            double ratio;
            if (denominator > eps)
            {
                double cosmin = (moms.mu20 - moms.mu02) / denominator;
                double sinmin = 2 * moms.mu11 / denominator;
                double cosmax = -cosmin;
                double sinmax = -sinmin;

                double imin = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmin - moms.mu11 * sinmin;
                double imax = 0.5 * (moms.mu20 + moms.mu02) - 0.5 * (moms.mu20 - moms.mu02) * cosmax - moms.mu11 * sinmax;
                ratio = imin / imax;
            }
            else
            {
                ratio = 1;
            }

            if (ratio < params.minInertiaRatio || ratio >= params.maxInertiaRatio)
                continue;
            }

        if (params.filterByConvexity)
        {
            vector < Point > hull;
            convexHull(Mat(contours[contourIdx]), hull);
            double area = contourArea(Mat(contours[contourIdx]));
            double hullArea = contourArea(Mat(hull));
            double ratio = area / hullArea;
            if (ratio < params.minConvexity || ratio >= params.maxConvexity)
                continue;
        }

        center = KeyPoint(moms.m10 / moms.m00, moms.m01 / moms.m00, 1);

        if (params.filterByColor)
        {
            if (binaryImage.at<uchar> (cvRound(center.pt.y), cvRound(center.pt.x)) != params.blobColor)
                continue;
        }

        //compute blob radius
        {
            vector<double> dists;
            for (size_t pointIdx = 0; pointIdx < contours[contourIdx].size(); pointIdx++)
            {
                Point pt = contours[contourIdx][pointIdx];
                int xDiff = abs(center.pt.x - pt.x);
                int yDiff = abs(center.pt.y - pt.y);
                double dist = sqrt(pow(xDiff, 2) + pow(yDiff, 2));
                dists.push_back(dist);
            }
            std::sort(dists.begin(), dists.end());
            int centerRadius = (dists[(dists.size() - 1) / 2] + dists[dists.size() / 2]) / 2.;
            center.size = centerRadius;
        }

        centers.push_back(center);

#ifdef DEBUG_BLOB_DETECTOR
        //    circle( keypointsImage, center.location, 1, Scalar(0,0,255), 1 );
#endif
    }
#ifdef DEBUG_BLOB_DETECTOR
    //  imshow("bk", keypointsImage );
    //  waitKey();
#endif
}

#endif

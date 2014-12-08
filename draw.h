#ifndef DRAW_H
#define DRAW_H
#endif // DRAW_H

#include "opencv2/features2d/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//from opencv-feature-detector, ported to opencv c++
void rotated_rect(cv::Mat im, const cv::RotatedRect & rot_rect, cv::Scalar color);

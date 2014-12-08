#include "draw.h"

//from opencv-feature-detector, ported to opencv c++
void rotated_rect(cv::Mat im, const cv::RotatedRect & rot_rect, cv::Scalar color) {
    cv::Point2f box_vtx[4];
    rot_rect.points(box_vtx);

    // copied shamelessly from minarea.c
    // it initialize to the last point, then connect to point 0, point 1, point 2 pair-wise
    cv::Point pt0, pt;
    pt0.x = round(box_vtx[3].x);
    pt0.y = round(box_vtx[3].y);
    for(int i = 0; i < 4; i++ )
    {
        pt.x = round(box_vtx[i].x);
        pt.y = round(box_vtx[i].y);
        cv::line(im, pt0, pt, color, 1, CV_AA, 0);
        pt0 = pt;
    }
}

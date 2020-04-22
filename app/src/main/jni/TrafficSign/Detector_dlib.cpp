//
// Created by badricio on 15/04/20.
//
#include "Detector_dlib.h"

#include <dlib/opencv.h>
#include "Debug.h"


Detector_dlib* Detector_dlib::m_detectorInstace = NULL;

Detector_dlib::~Detector_dlib()
{
    if(m_detectorInstace)
    {
        m_detectorInstace = NULL;
    }
}


Detector_dlib::Detector_dlib()
{
    dlib::deserialize("/sdcard/DriverAssistence/detector/detector_01_20_1600_plates.svm") >> detector;
}


std::vector<dlib::rectangle> Detector_dlib::detect_objects( cv::Mat img )
{
    dlib::array2d<dlib::bgr_pixel> dlibIm;
    dlib::assign_image(dlibIm, dlib::cv_image<dlib::bgr_pixel>(cvIplImage(img)));

    return detector(dlibIm);
}

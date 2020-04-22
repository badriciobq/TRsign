//
// Created by badricio on 15/04/20.
//

#include <dlib/image_processing.h>
#include <dlib/svm_threaded.h>
#include <dlib/data_io.h>
#include <opencv2/core/core.hpp>

#include <iostream>



#ifndef ADAS_DETECTOR_DLIB_H
#define ADAS_DETECTOR_DLIB_H


class Detector_dlib {

    public:
        virtual ~Detector_dlib();

        static Detector_dlib * GetInstance(){
            if(!m_detectorInstace){
                m_detectorInstace = new Detector_dlib();
            }
            return m_detectorInstace;
        }

        std::vector<dlib::rectangle> detect_objects( cv::Mat );

    private:
        Detector_dlib();

        typedef dlib::scan_fhog_pyramid<dlib::pyramid_down<6> > image_scanner_type;
        dlib::object_detector<image_scanner_type> detector;

        static Detector_dlib* m_detectorInstace;
};



#endif //ADAS_DETECTOR_DLIB_H

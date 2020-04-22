#include "adas.h"

#include "Debug.h"
#include "Utils.h"
#include "Detector_dlib.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


JavaVM *s_Jvm;

Detector_dlib *m_detectorDlib = NULL;

bool isInitialized = false;


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    if(!vm)
    {
        s_Jvm = vm;
    }

    return JNI_VERSION_1_6;
}


/*
 * Class:     com_example_adas_MainActivity
 * Method:    ProcessImage
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_example_adas_MainActivity_ProcessImage
  (JNIEnv *env, jobject, jlong src)
  {
    // Converte os endereços das matrizes retornadas pela camada java em objetos do c++.
    cv::Mat &mSrc = *((cv::Mat *) src);

    // Converte matriz para RGB (3 Canais devido ao fato da dlib usar bgr_pixel)
    cv::Mat mRGB;
    cv::cvtColor(mSrc, mRGB, cv::COLOR_RGBA2BGR);


    int x, y, width, height;

    int rows = mRGB.rows;
    int cols = mRGB.cols;

    std::vector<dlib::rectangle> detections;

    // Define a região de interesse
    x = cols/2+cols*0.2;
    y = rows*0.1;
    width = cols-x;
    height = rows-y-rows*0.25;
    cv::Rect roi = cv::Rect(x*0.9,y,width, height);


    if(m_detectorDlib)
    {
        std::vector<dlib::rectangle> objects = m_detectorDlib->detect_objects( mRGB(roi) );

        for ( size_t i = 0; i < objects.size(); i++ )
        {
            int x1 = objects[i].left();
            int y1 = objects[i].top();
            int x2 = objects[i].right();
            int y2 = objects[i].bottom();

            /*Matriz temporaria para salvar a placa em arquivo.*/
            // TODO: Validar se o Roi está out off bound.

            if(x1 > 0 && y1 > 0 && x2 < mRGB(roi).cols && y2 < mRGB(roi).rows)
            {
                cv::Mat tmp = mSrc(roi).clone();
                store_image( tmp( cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)) ).clone() );

                // Libera memória da placa temporparia
                tmp.release();
            }


            cv::rectangle(mSrc(roi), cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255,0,0), 2);
        }
    }
    else
    {
        LOGE("There is not a detector instantiated");
    }



    cv::rectangle(mSrc, roi, cv::Scalar(0,255,0), 1);

    cv::line(mSrc, cv::Point(0,(rows/2)+y), cv::Point(cols,(rows/2)+y), cv::Scalar(255,0,0), 1);
    cv::circle(mSrc, cv::Point((cols/2),(rows/2)+y), 10, cv::Scalar(255,0,0), 1);

    mRGB.release();

    return;
  }

/*
 * Class:     com_example_adas_MainActivity
 * Method:    Initialize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_adas_MainActivity_Initialize
  (JNIEnv * env, jobject obj)

  {
    if(isInitialized) {
        return JNI_TRUE;
    }

    m_detectorDlib = Detector_dlib::GetInstance();
    if( !m_detectorDlib)
    {
        return JNI_FALSE;
    }

    isInitialized = true;

    return JNI_TRUE;

  }

/*
 * Class:     com_example_adas_MainActivity
 * Method:    Finalize
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_example_adas_MainActivity_Finalize
  (JNIEnv * env, jobject obj)
  {
    isInitialized = false;

    return JNI_TRUE;
  }

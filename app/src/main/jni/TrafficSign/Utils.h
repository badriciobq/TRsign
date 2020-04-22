#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/time.h>
#include <sstream>
#include <vector>


// Remover espaços em branco
char * deblank(char *str)
{
    char *out = str, *put = str;

    for(; *str != '\0'; ++str)
    {
        if(*str != ' ')
            *put++ = *str;
    }
    *put = '\0';

    return out;
}

// Gravar imagem em arquivo
void store_image(cv::Mat toFile)
{
    // Write image in file to training data
    cv::cvtColor(toFile, toFile, cv::COLOR_BGR2RGB);

    struct timeval t_time;
    gettimeofday(&t_time, NULL);

    std::stringstream ss;
    ss << "/sdcard/DriverAssistence/Plate/" << (int) t_time.tv_sec << "_" << t_time.tv_usec << ".jpg";

    cv::imwrite(ss.str().c_str(), toFile);

    toFile.release();
}

// Gravar imagem em arquivo
void store_image_2(cv::Mat toFile, char *vel)
{
    // Write image in file to training data
    cv::cvtColor(toFile, toFile, cv::COLOR_BGR2RGB);

    struct timeval t_time;
    gettimeofday(&t_time, NULL);

    std::stringstream ss;
    ss << "/sdcard/DriverAssistence/bg/" << (int) t_time.tv_sec << "_" << t_time.tv_usec << ".jpg";


    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95);

    //cv::imwrite(ss.str().c_str(), toFile, compression_params);

    toFile.release();
}
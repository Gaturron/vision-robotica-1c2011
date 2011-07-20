#ifndef _VISION
  #define _VISION

#include <cv.h> /* required to use OpenCV */
#include <highgui.h> /* required to use OpenCV's highgui */
#include "stdio.h"
#include <string>
#include <iostream>
#include <elas.h>
   
using namespace std;
using namespace cv;

void moveImageH(const Mat& Im, int offset, Mat& res);
void moveImageV(const Mat& Im, int offset, Mat& res);
void map2Color(cv::Mat &left_disparities, cv::Mat &mat);
void alignImages(const Mat& img1_rect, const Mat& img2_rect, int* widthValue, int* heightValue );
void disparity(const Mat& img1, const Mat& img2, const Mat& mapx1, const Mat& mapy1, const Mat& mapx2, const Mat& mapy2,  Mat& mapDis1, Mat& mapDis2);
void initSize(String option, String nameFile, cv::Size& size);
void ErrorParametros();
void capturarImagenesDesdeVideo(Mat& img_left, Mat& img_right, int numFrame);

#endif

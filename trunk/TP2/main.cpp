#include <cv.h> /* required to use OpenCV */
#include <highgui.h> /* required to use OpenCV's highgui */
#include "stdio.h"
#include <string>
#include <iostream>
   
using namespace std;
using namespace cv;

void moveImage(const Mat& Im1, const Mat& Im2, int offset, Mat& res){

	//muevo Im2 un cacho para la derecha
	cv::Mat mat(Im2.rows, Im2.cols+offset, CV_32F);
	for(int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			if(j < Im2.cols){			
				mat.at<float>(i,j) = Im2.at<float>(i,j);
			}else{
				//mat.at<double>(i,j) = Im2.at<double>(i,j-offset);		
				mat.at<float>(i,j) = 0;
			}			
		}	
	}
	cv::namedWindow("imagen3", CV_WINDOW_AUTOSIZE);
	cv::imshow("imagen3",mat);
	
	//hago addWeighted	
    //cv::addWeighted(Im1, 0.5, mat, 0.5, 1, res);
    //cv::imshow("Imagen Res",res);

}

int main(int argc, char *argv[]) {

	if (argc != 4){
		cout << "Error en los parametros" << endl;
		return 1;
	}

	string filename = argv[1];
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	
	if (!fs.isOpened()){
        cout << "Error al leer el archivo " << filename << endl;
        return 1;
	}    
    
    cv::FileNode root = fs.root();
    cv::Mat p1,p2, dist1, dist2, r, t;

    root["P1"] >> p1;
    root["P2"] >> p2;
    root["dist1"] >> dist1;
    root["dist2"] >> dist2;
    root["R"] >> r;
    root["T"] >> t;

    cv::Mat img1, img2, img1_rect, img2_rect;   
   	//imagenes
    img2 = cv::imread(argv[2],1);
    img1 = cv::imread(argv[3],1);
	cv::Size size = img1.size();
	//size = cv::Size(img1.width, img1->height);

	//Matrices ya rectificadas
	cv::Mat P1, P2, R1, R2, Q;
	double alpha = 0.0;
	cv::Rect roi1, roi2;
   
	cv::stereoRectify(p1, dist1, p2, dist2, size, r, t, R1, R2, P1, P2, Q, alpha, size, &roi1, &roi2, cv::CALIB_ZERO_DISPARITY); 
	//void stereoRectify(const Mat&  cameraMatrix1, const Mat&  distCoeffs1, const Mat&  cameraMatrix2, const Mat&  distCoeffs2, Size imageSize, const Mat&  R, const Mat&  T, Mat&  R1, Mat&  R2, Mat&  P1, Mat&  P2, Mat&  Q, double alpha, Size newImageSize=Size(), Rect*  roi1=0, Rect* roi2=0, int flags=CALIB_ZERO_DISPARITY)

	cv::Mat mapx1, mapy1, mapx2, mapy2;
	cv::initUndistortRectifyMap(p1, dist1, R1, P1, size, CV_32FC1, mapx1, mapy1);
	cv::initUndistortRectifyMap(p2, dist2, R2, P2, size, CV_32FC1, mapx2, mapy2);
	//void initUndistortRectifyMap(const Mat&  cameraMatrix, const Mat&  distCoeffs, const Mat&  R, const Mat&  newCameraMatrix, Size size, int m1type, Mat&  map1, Mat&  map2)¶
  
  
	cv::remap(img1, img1_rect, mapx1, mapy1, cv::INTER_LINEAR);
	cv::remap(img2, img2_rect, mapx2, mapy2, cv::INTER_LINEAR);
	//void remap(const Mat&  src, Mat&  dst, const Mat&  map1, const Mat&  map2, int interpolation, int borderMode=BORDER_CONSTANT, const Scalar& borderValue=Scalar())¶
  
	cv::namedWindow("imagen1", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("imagen2", CV_WINDOW_AUTOSIZE);
	cv::imshow("imagen1",img1_rect);
	cv::imshow("imagen2",img2_rect);
	cv::waitKey(0);
  
	cv::Mat res(img1_rect.rows*2, img1_rect.cols*2, CV_32F);
	cv::Mat res1(img1_rect.rows*2, img1_rect.cols*2, CV_32F);
	
	cv::namedWindow("Imagen Res", CV_WINDOW_AUTOSIZE);

	int value = 50;

	cv::createTrackbar("trackbar1", "Imagen Res", &value, 100);

	cv::addWeighted(img1_rect, 0.5, img2_rect, 0.5, 1, res);
     
	cv::imshow("Imagen Res",res);
	
	moveImage(img1_rect, img1_rect, 10, res);

//	while(cv::waitKey(0)){
		
//	}

	cv::waitKey(0);
	
	return 0;
}

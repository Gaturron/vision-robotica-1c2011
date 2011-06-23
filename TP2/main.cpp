    #include <cv.h> /* required to use OpenCV */
    #include <highgui.h> /* required to use OpenCV's highgui */
    #include "stdio.h"
    #include <string>
    #include <iostream>
   
   using namespace std;
   using namespace cv;

int main(int argc, char *argv[]) {

	if (argc != 2){
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
    img2 = cv::imread("Image1_left.tif",1);
    img1 = cv::imread("Image1_right.tif",1);
    cv::Size size = img1.size();
//  size = cv::Size(img1.width, img1->height);

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
  
   //cv::MatIterator_<double> myIter = P1.begin<double>(), it_end = P1.end<double>();
   
  // for (; myIter!=it_end; ++myIter){
  //  double v = *myIter;
  //  cout << v << endl;
  // } 
  
   return 0;
}

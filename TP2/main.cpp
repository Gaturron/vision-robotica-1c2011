#include <cv.h> /* required to use OpenCV */
#include <highgui.h> /* required to use OpenCV's highgui */
#include "stdio.h"
#include <string>
#include <iostream>
#include <elas.h>
   
using namespace std;
using namespace cv;

void moveImageH(const Mat& Im, int offset, Mat& res){

	//muevo Im2 un cacho para la derecha
	cv::Mat_<Vec3b> mat(Im.rows, Im.cols, CV_8UC3);
	
	for(int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			for (int c=0; c<3; c++){
			    if(j >= offset){			
				    mat.at<Vec3b>(i,j)[c] = Im.at<Vec3b>(i,j-offset)[c];
			    }else{
				    mat.at<Vec3b>(i,j)[c] = 0;
			    }			
			}
		}	
	}
	
	res = mat;
	//hago addWeighted	
    //cv::imshow("Imagen Res",res);

}

void moveImageV(const Mat& Im, int offset, Mat& res){

	//muevo Im2 un cacho para la derecha
	cv::Mat_<Vec3b> mat(Im.rows, Im.cols, CV_8UC3);
	
	for(int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			for (int c=0; c<3; c++){
			    if(i >= offset){			
				    mat.at<Vec3b>(i,j)[c] = Im.at<Vec3b>(i - offset,j)[c];
			    }else{
				    mat.at<Vec3b>(i,j)[c] = 0;
			    }			
			}
		}	
	}
	
	res = mat;
	//hago addWeighted	
    //cv::imshow("Imagen Res",res);

}

void alignImages(const Mat& img1_rect, const Mat& img2_rect, int* widthValue, int* heightValue ){
    cv::Mat_<Vec3b> res(img1_rect.rows*2, img1_rect.cols*2, CV_8UC3);
	cv::Mat_<Vec3b> res1(img1_rect.rows*2, img1_rect.cols*2, CV_8UC3);
	cv::Mat_<Vec3b> output(img1_rect.rows*2, img1_rect.cols*2, CV_8UC3);
	
	cv::namedWindow("imgRes", CV_WINDOW_AUTOSIZE);
    int valueh = 128;
    cv::createTrackbar("horizontal", "imgRes", &valueh, 256);
    
    int valuev = 128;
    cv::createTrackbar("vertical", "imgRes", &valuev, 256);
	
//	cv::addWeighted(img1_rect, 0.5, img2_rect, 0.5, 1, res);
//	cv::namedWindow("imagen3", CV_WINDOW_AUTOSIZE);
//	cv::imshow("imagen3",res);

	while(cv::waitKey(200)<0){
		moveImageH(img1_rect, valueh - 128, res);
		moveImageV(res, valuev - 128, res1);
	    cv::addWeighted(img2_rect, 0.5, res1, 0.5, 1, output);
		cv::imshow("imgRes",output);
	}
	
	*widthValue = valueh;
	*heightValue = valuev;
}

void disparity(const Mat& img1, const Mat& img2, const Mat& mapx1, const Mat& mapy1, const Mat& mapx2, const Mat& mapy2,  Mat& mapDis1, Mat& mapDis2){

    cv::Mat_<Vec3b> img1_rect, img2_rect;
    
    cv::remap(img1, img1_rect, mapx1, mapy1, cv::INTER_LINEAR);
	cv::remap(img2, img2_rect, mapx2, mapy2, cv::INTER_LINEAR);
	//void remap(const Mat&  src, Mat&  dst, const Mat&  map1, const Mat&  map2, int interpolation, int borderMode=BORDER_CONSTANT, const Scalar& borderValue=Scalar())¶
  
	//cv::namedWindow("imagen1", CV_WINDOW_AUTOSIZE);
	//cv::namedWindow("imagen2", CV_WINDOW_AUTOSIZE);
	//cv::imshow("imagen1",img1_rect);
	//cv::imshow("imagen2",img2_rect);
	//cv::waitKey(0);
    
    //Interfaz grafica para alinear las imagenes
    //esta funcion devuelve los valores (horizontal y vertical respectivamente) a desplazar cada imagen
    int valueh, valuev;
    alignImages(img1_rect, img2_rect, &valueh, &valuev);
    
	cout<<valueh<<endl;
	cout<<valuev<<endl;
	
	Mat_<uchar> grayIm1_rect, grayIm2_rect;
	
	cv::cvtColor(img1_rect, grayIm1_rect, CV_RGB2GRAY);
	cv::cvtColor(img2_rect, grayIm2_rect, CV_RGB2GRAY);
	
	cv::namedWindow("imagen3", CV_WINDOW_AUTOSIZE);
	cv::imshow("imagen3",grayIm1_rect);
	int dims[3];
	dims[0] = grayIm1_rect.cols;
	dims[1] = grayIm1_rect.rows;
	dims[2] = grayIm1_rect.cols;
	// process
  Elas::parameters param;
  param.postprocess_only_left = false;
  Elas elas(param);
  elas.process(grayIm1_rect,grayIm2_rect,mapDis1,mapDis2,dims);


	cv::waitKey(0);
}

int main(int argc, char *argv[]) {

	if (argc != 5){
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

    cv::Mat_<Vec3b> img1, img2, imgDisp1, imgDisp2;   
   	//imagenes
    img2 = cv::imread(argv[3],1);
    img1 = cv::imread(argv[4],1);
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
    
    //HASTA ACA ES LO MISMO PARA IMAGENES Y VIDEO. LO QUE VIENE SE REPITE POR CADA IMAGEN DEL VIDEO
    
    if((String) argv[2] == "-i"){
        disparity(img1, img2, mapx1, mapy1, mapx2, mapy2, imgDisp1, imgDisp2);
    }
    else if((String) argv[2] == "-v"){
        
    }
    else{
        cout<<"no entreee"<<endl;
    }
    
	
	
	return 0;
}

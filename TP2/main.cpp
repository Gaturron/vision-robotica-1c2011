#include <cv.h> /* required to use OpenCV */
#include <highgui.h> /* required to use OpenCV's highgui */
#include "stdio.h"
#include <string>
#include <iostream>
#include <elas.h>
   
using namespace std;
using namespace cv;

int alignDoIt = false;
int valueh, valuev;

void moveImageH(const Mat& Im, int offset, Mat& res){

	//mueve la imagen Im horizontalmente y lo guarda en res
	cv::Mat_<Vec3b> mat(Im.size(), CV_8UC3);
	
	for(int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			for (int c=0; c<3; c++){
			    if(j >= offset){			
					if(0 <= j-offset && j-offset < Im.cols)
				    	mat.at<Vec3b>(i,j)[c] = Im.at<Vec3b>(i,j-offset)[c];
					else
						mat.at<Vec3b>(i,j)[c] = 0;
			    }else{
				    mat.at<Vec3b>(i,j)[c] = 0;
			    }			
			}
		}	
	}
	
	res = mat;
}

void moveImageV(const Mat& Im, int offset, Mat& res){

	//mueve la imagen Im verticalmente y lo guarda en res
	cv::Mat_<Vec3b> mat(Im.size(), CV_8UC3);
	
	for(int i = 0; i < mat.rows; i++) {
		for(int j = 0; j < mat.cols; j++) {
			for (int c=0; c<3; c++){
			    if(i >= offset){

					if(0 <= i - offset && i - offset < Im.rows)			
					    mat.at<Vec3b>(i,j)[c] = Im.at<Vec3b>(i - offset,j)[c];
					else			    
						mat.at<Vec3b>(i,j)[c] = 0;

				}else{
				    mat.at<Vec3b>(i,j)[c] = 0;
			    }			
			}
		}	
	}
    
	res = mat;
}

void map2Color(cv::Mat &left_disparities, cv::Mat &mat){
    int min_disparity = 0;
    
    for (int i = 0; i < mat.rows; i++) {
        for (int j = 0; j < mat.cols; j++) {
            //cout<<"disparity: "<<left_disparities.at<float>(i,j)<<endl;
            if (left_disparities.at<float>(i,j) > min_disparity) {
                
                float val = min(left_disparities.at<float>(i,j)*0.01f,1.0f);
                //cout<<"val: "<<val<<endl;
                if (val < 0) {
                     mat.at<Vec3b>(i,j)[0] = 0;
                     mat.at<Vec3b>(i,j)[1] = 0;
                     mat.at<Vec3b>(i,j)[2] = 0;
                } else {
                    float h2 = 6.0f * (1.0f - val);
                    unsigned char x  = (unsigned char)((1.0f - fabs(fmod(h2, 2.0f) - 1.0f))*255);
                    if (0 <= h2&&h2<1) {
                        mat.at<Vec3b>(i,j)[0] = 255;
                        mat.at<Vec3b>(i,j)[1] = x;
                        mat.at<Vec3b>(i,j)[2] = 0;
                    }
                    else if (1<=h2&&h2<2)  {
                        mat.at<Vec3b>(i,j)[0] = x;
                        mat.at<Vec3b>(i,j)[1] = 255;
                        mat.at<Vec3b>(i,j)[2] = 0;
                    }
                    else if (2<=h2&&h2<3)  {
                        mat.at<Vec3b>(i,j)[0] = 0;
                        mat.at<Vec3b>(i,j)[1] = 255;
                        mat.at<Vec3b>(i,j)[2] = x;
                    }
                    else if (3<=h2&&h2<4)  {
                        mat.at<Vec3b>(i,j)[0] = 0;
                        mat.at<Vec3b>(i,j)[1] = x;
                        mat.at<Vec3b>(i,j)[2] = 255;
                    }
                    else if (4<=h2&&h2<5)  {
                        mat.at<Vec3b>(i,j)[0] = x;
                        mat.at<Vec3b>(i,j)[1] = 0;
                        mat.at<Vec3b>(i,j)[2] = 255;
                    }
                    else if (5<=h2&&h2<=6) {
                        mat.at<Vec3b>(i,j)[0] = 255;
                        mat.at<Vec3b>(i,j)[1] = 0;
                        mat.at<Vec3b>(i,j)[2] = x;
                    }
                }
            }
            else {
                mat.at<Vec3b>(i,j)[0] = 0;
                mat.at<Vec3b>(i,j)[1] = 0;
                mat.at<Vec3b>(i,j)[2] = 0;
            }
        }
    }
}

void alignImages(const Mat& img1_rect, const Mat& img2_rect, int* widthValue, int* heightValue ){
    cv::Mat_<Vec3b> res(img1_rect.cols*2, img1_rect.rows*2, CV_8UC3);
	cv::Mat_<Vec3b> res1(img1_rect.cols*2, img1_rect.rows*2, CV_8UC3);
	cv::Mat_<Vec3b> output(img1_rect.cols*2, img1_rect.rows*2, CV_8UC3);
	
	cv::namedWindow("imgRes", CV_WINDOW_AUTOSIZE);
    int setvalueh = 128;
    cv::createTrackbar("horizontal", "imgRes", &setvalueh, 256);
    
    int setvaluev = 128;
    cv::createTrackbar("vertical", "imgRes", &setvaluev, 256);

	while(cv::waitKey(200)<0){
		moveImageH(img1_rect, setvalueh - 128, res);
		moveImageV(res, setvaluev - 128, res1);
	    cv::addWeighted(img2_rect, 0.5, res1, 0.5, 1, output);
		cv::imshow("imgRes",output);
	}
	
	*widthValue = setvalueh;
	*heightValue = setvaluev;
}

void disparity(const Mat& img1, const Mat& img2, const Mat& mapx1, const Mat& mapy1, const Mat& mapx2, const Mat& mapy2,  Mat& mapDis1, Mat& mapDis2){

    cv::Mat_<Vec3b> img1_rect, img2_rect;
    
    cv::remap(img1, img1_rect, mapx1, mapy1, cv::INTER_LINEAR);
	cv::remap(img2, img2_rect, mapx2, mapy2, cv::INTER_LINEAR);
	//void remap(const Mat&  src, Mat&  dst, const Mat&  map1, const Mat&  map2, int interpolation, int borderMode=BORDER_CONSTANT, const Scalar& borderValue=Scalar())
    
    cv::Mat_<Vec3b> res1(img1_rect.cols, img1_rect.rows, CV_8UC3);
    cv::Mat_<Vec3b> temp1(img1_rect.cols, img1_rect.rows, CV_8UC3);
    cv::Mat_<Vec3b> res2(img2_rect.cols, img2_rect.rows, CV_8UC3);
    cv::Mat_<Vec3b> temp2(img2_rect.cols, img2_rect.rows, CV_8UC3);
    
    if(!alignDoIt){
        cout<<"Configuro los valores de desplazamiento"<<endl;
        
        //Interfaz grafica para alinear las imagenes
        //esta funcion devuelve los valores (horizontal y vertical respectivamente) a desplazar cada imagen
        alignImages(img1_rect, img2_rect, &valueh, &valuev);
        
        moveImageH(img1_rect, valueh - 128, temp1);
        moveImageV(temp1, valuev - 128, res1);

        //moveImageH(img2_rect, valueh - 128, temp2);
        //moveImageV(temp2, valuev - 128, res2);
        
        alignDoIt = true;
    }
    else{
        cout<<"uso los valores de desplazamiento definidos"<<endl;
        moveImageH(img1_rect, valueh - 128, temp1);
        moveImageV(temp1, valuev - 128, res1);

        //moveImageH(img2_rect, valueh - 128, temp2);
        //moveImageV(temp2, valuev - 128, res2);
    }
    
	cout<<valueh<<endl;
	cout<<valuev<<endl;
	
	Mat_<uchar> grayIm1_rect, grayIm2_rect;
    Mat_<uchar> grayIm1_rect2, grayIm2_rect2;

    cv::cvtColor(res1, grayIm1_rect, CV_RGB2GRAY);
    cv::cvtColor(img2_rect, grayIm2_rect, CV_RGB2GRAY);
	
	int32_t dims[3];
	dims[0] = grayIm1_rect.cols;
	dims[1] = grayIm1_rect.rows;
	dims[2] = grayIm1_rect.cols;
	cout << mapDis1.cols << " " << mapDis1.rows << endl;
    
    equalizeHist(grayIm1_rect, grayIm1_rect2);
    equalizeHist(grayIm2_rect, grayIm2_rect2);
    
    cv::namedWindow("hist1", CV_WINDOW_AUTOSIZE);
    cv::imshow("hist1",grayIm1_rect2);
    
    cv::namedWindow("hist2", CV_WINDOW_AUTOSIZE);
    cv::imshow("hist2",grayIm2_rect2);
    
	// process
    Elas::parameters param;
    //param.postprocess_only_left = false;
    Elas elas(param);
    elas.process(grayIm1_rect.data,grayIm2_rect.data,(float*)mapDis1.data,(float*)mapDis2.data,dims);

    cv::Mat_<Vec3b> l1 (mapDis1.size()), l2 (mapDis2.size());
    
    map2Color(mapDis1, l1);
    mapDis1 = l1;
    
    map2Color(mapDis2, l2);
    mapDis2 = l2;
}

void initSize(String option, String nameFile, cv::Size& size){    
    if(option == "-i"){
        cv::Mat_<Vec3b> img;
        
        img = cv::imread(nameFile);
        size = img.size();
    }
    else if(option == "-v"){
        VideoCapture cap;
        cap.open(nameFile);
        cv::Mat frame;
        cap >> frame;
        size = frame.size();
    }
    else{
        cout << "Error en los parametros" << endl;
    }
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

    
    cv::Size size;
    initSize(argv[2], argv[3], size);
    
    cout<<"ancho: "<<size.width<<endl;
    cout<<"alto: "<<size.height<<endl;

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
        cv::Mat_<Vec3b> img1, img2;
        //imagenes
        img2 = cv::imread(argv[3],1);
        img1 = cv::imread(argv[4],1);
        cv::Mat_<float> imgDisp1(size);
        cv::Mat_<float> imgDisp2(size);   
        
        disparity(img1, img2, mapx1, mapy1, mapx2, mapy2, imgDisp1, imgDisp2);
        
        cv::imwrite("output1.tiff", imgDisp1);
        cv::imwrite("output2.tiff", imgDisp2);
        
        cv::namedWindow("imagencolor1", CV_WINDOW_AUTOSIZE);
        cv::imshow("imagencolor1",imgDisp1);
        
        cv::namedWindow("imagencolor2", CV_WINDOW_AUTOSIZE);
        cv::imshow("imagencolor2",imgDisp2);

        cv::waitKey(0);
        
    }
    else if((String) argv[2] == "-v"){
		//levantamos los videos		
		VideoCapture cap1, cap2;
        
        double fps = 10;
        CvVideoWriter *writer1 = cvCreateVideoWriter("output1.avi",CV_FOURCC('M', 'J', 'P', 'G'),fps,size);
        CvVideoWriter *writer2 = cvCreateVideoWriter("output2.avi",CV_FOURCC('M', 'J', 'P', 'G'),fps,size);


		if(argc > 2) cap1.open(string(argv[3])); 
		else cap1.open(0);
	
		if(argc > 3) cap2.open(string(argv[4])); 
		else cap2.open(0);
		 
		namedWindow("videoIzq", CV_WINDOW_AUTOSIZE);
		namedWindow("videoIzqDisp", CV_WINDOW_AUTOSIZE);
		//namedWindow("videoDer", 1);
        
        cv::Mat_<Vec3b> frame1, frame2;
        
		for(;;){
            cv::Mat_<float> imgDisp1(size);
            cv::Mat_<float> imgDisp2(size);
            //leemos el frame de cada video
            cap1 >> frame1; 
            if(!frame1.data){
                cout<<"No hay mas frames primer video"<<endl;
                break;
            }
            cap2 >> frame2; 
            if(!frame2.data){
                cout<<"No hay mas frames segundo video"<<endl;
                break;
            }
            
            //cout<<"ancho frame: "<<frame1.cols<<endl;
            //cout<<"alto frame: "<<frame1.rows<<endl;
            
            disparity(frame1, frame2, mapx1, mapy1, mapx2, mapy2, imgDisp1, imgDisp2);
            
            IplImage ipl_img1 = imgDisp1;
            IplImage ipl_img2 = imgDisp2;
            
            cvWriteFrame(writer1, &ipl_img1);
            cvWriteFrame(writer2, &ipl_img2);
            
			cv::imshow("videoIzq", frame1); 
			imshow("videoIzqDisp", imgDisp1);
            
            cv::imshow("videoDer", frame2); 
			imshow("videoDerDisp", imgDisp2); 
			
            if(waitKey(30) > 0){
                cout<<"Salio por falta de tiempo"<<endl;
                break;
            }
		}
        cout<<"termino el video"<<endl;
        cv::waitKey(0);

    }
    else{
        cout<<"Error en los parametros"<<endl;
    }
    
	return 0;
}

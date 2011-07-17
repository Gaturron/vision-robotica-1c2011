#include "vision.h"

int alignDoIt = false;
int valueh, valuev;

void ErrorParametros(){
    cout<<"Error en los parametros.\n Se usa:\n\n./main ARCHIVO_PARAMETROS OPCIONES ARCHIVO_IZQ ARCHIVO_DER\ndonde dice:\n\nARCHIVO_PARAMETROS: es el archivo que contiene los parámetros de la calibración de la cámara.\nOPCIONES: -i Imagenes -v Video\nARCHIVO_IZQ: es el archivo correspondiente a la camara izquierda(Sea una imagen o video)\nARCHIVO_DER: es el archivo correspondiente a la camara derecha(Sea una imagen o video)\nejemplo: \nPara imagenes:\n./main parameters.xml -i Image0_left.tif Image0_right.tif \nPara videos:\n./main parameters.xml -v cam_left.avi cam_right.avi"<<endl;
}

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
                if (val <= 0) {
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
	//cout << mapDis1.cols << " " << mapDis1.rows << endl;
    
    /*equalizeHist(grayIm1_rect, grayIm1_rect2);
    equalizeHist(grayIm2_rect, grayIm2_rect2);
    
    cv::namedWindow("hist1", CV_WINDOW_AUTOSIZE);
    cv::imshow("hist1",grayIm1_rect2);
    
    cv::namedWindow("hist2", CV_WINDOW_AUTOSIZE);
    cv::imshow("hist2",grayIm2_rect2);*/
    
	// process
    Elas::parameters param;
    //param.postprocess_only_left = false;
    Elas elas(param);
    elas.process(grayIm2_rect.data,grayIm1_rect.data,(float*)mapDis1.data,(float*)mapDis2.data,dims);

    /*cv::Mat_<Vec3b> l1 (mapDis1.size()), l2 (mapDis2.size());
    
    map2Color(mapDis1, l1);
    mapDis1 = l1;
    
    map2Color(mapDis2, l2);
    mapDis2 = l2;*/
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
        ErrorParametros();
    }
}

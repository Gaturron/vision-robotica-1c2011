#include "vision.h"

int main(int argc, char *argv[]) {

	if (argc != 5){
		ErrorParametros();
		return 1;
	}

	string filename = argv[1];
	cv::FileStorage fs(filename, cv::FileStorage::READ);
	
	if (!fs.isOpened()){
        ErrorParametros();
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
        img2 = cv::imread(argv[4],1);
        img1 = cv::imread(argv[3],1);
        cv::Mat_<float> imgDisp1(size);
        cv::Mat_<float> imgDisp2(size);   
        
        disparity(img1, img2, mapx1, mapy1, mapx2, mapy2, imgDisp1, imgDisp2);
        
        cv::imwrite("output1.tiff", imgDisp1);
        cv::imwrite("output2.tiff", imgDisp2);
        
        cv::namedWindow("imagencolor1", CV_WINDOW_AUTOSIZE);
        cv::imshow("imagencolor1",imgDisp1);
        
        //Muestro el mapa de disparidad de la camara derecha
        //cv::namedWindow("imagencolor2", CV_WINDOW_AUTOSIZE);
        //cv::imshow("imagencolor2",imgDisp2);

        cv::waitKey(0);
        
    }
    else if((String) argv[2] == "-v"){
		//levantamos los videos		
		VideoCapture cap1, cap2;
        
        double fps = 30;
        CvVideoWriter *writer1 = cvCreateVideoWriter("output1.avi",CV_FOURCC('M', 'J', 'P', 'G'),fps,size);
        CvVideoWriter *writer2 = cvCreateVideoWriter("output2.avi",CV_FOURCC('M', 'J', 'P', 'G'),fps,size);


		if(argc > 2) cap1.open(string(argv[3])); 
		else cap1.open(0);
	
		if(argc > 3) cap2.open(string(argv[4])); 
		else cap2.open(0);
		 
		//namedWindow("videoIzq", CV_WINDOW_AUTOSIZE);
		//namedWindow("videoIzqDisp", CV_WINDOW_AUTOSIZE);
		//namedWindow("videoDer", 1);
        
        cv::Mat_<Vec3b> frame1, frame2;
        int cantFrames = 0;
		for(;;){
            cantFrames ++;
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
			cv::imshow("videoIzqDisp", imgDisp1);
            
            //Muestro el video correspondiente al mapa de disparidad deracha
            //cv::imshow("videoDer", frame2); 
			//imshow("videoDerDisp", imgDisp2); 
			
            if(waitKey(30) > 0){
                cout<<"Usted ha decidio salir"<<endl;
                break;
            }
		}
        cout<<cantFrames<<endl;
        cout<<"termino el video"<<endl;
        cv::waitKey(0);

    }
    else{
        ErrorParametros();
    }
    
	return 0;
}

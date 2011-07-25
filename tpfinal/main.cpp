#include "vision.h"
const int AVANZAR = 0;
const int ANGULO = 1;
int* tipoMovs;
double* movs;
int numMov = 0;
double posActualx = 0;
double posActualy = 0;

void tomarImagenes(Mat& img_izq, Mat& img_der){
    VideoCapture cap1, cap2;
    
    cv::Mat_<Vec3b> frame_izq, frame_der;
    
    frame_der = cv::imread("/home/aolmedo/imgtpfinal/rightcam/rightcam_300.tiff",1);
    frame_izq = cv::imread("/home/aolmedo/imgtpfinal/leftcam/leftcam_300.tiff",1);
    
    //cap1.open(0);
    //cap2.open(1);
    
    //cap1 >> frame_izq;
    //cap2 >> frame_der;
    
    img_izq = frame_izq;
    img_der = frame_der;
    
    //cv::imwrite("test1.tiff", frame_izq);
    //cv::imwrite("test1.tiff", frame_der);
}

void vectorDistancia(Mat& roi, double* vector){
    Size size = roi.size();

    double porcentaje;
	double sum;
	double disparity;
    for(int j = 0; j < roi.cols; j++){
        sum = 0.0;
        for(int i = 0; i < roi.rows; i++){
			//aca hay que castear bien y hacer el promedio
            disparity = (double) roi.at<float>(i,j);
            sum += disparity;
		}
        porcentaje = sum / roi.rows;
		vector[j] = porcentaje;
    }
}

int calcularAnchoRobot(double* distancias, int length){
    double promedio, sum = 0;
    for(int i = 0; i < length; i++){
        if(distancias[i] >= 0){
            sum += distancias[i];
        }
    }
    promedio = sum / length;
    return (int) (length / promedio);
//	return 1;
}

double calcularAnguloGiro(int indexDirection){
    //dado el x donde f(x) indica la mejor salida tenemos que calcular el angulo de giro
    //este angulo ira de 0 a 90 o de -45 a 45 grados.
    double angulo = 0.0;    
    if(indexDirection < 0){
        //politica de escape
    }else{
        int temp = indexDirection - 320;
        double RelAnguloIndex = (double) 90.0/640.0;    
        angulo = temp * RelAnguloIndex;    
    }
    cout<<"angulo: "<<angulo<<endl;
    tipoMovs[numMov] = ANGULO;
    movs[numMov] = angulo;
    return angulo;
}

double calcularDistanciaArecorrer(double mejorSalida, double max){
    //tenemos que transformar el valor de disparidad en distancia
    //(igual no sabemos como se le indica la distancia a recorrer al robot)
    double distance;
    double coeficiente = (mejorSalida/max);
    if( coeficiente < 0.01){
        coeficiente = 0.01;
    }
    distance = 10 / coeficiente;
    cout<<"distancia: "<<distance<<endl;
    tipoMovs[numMov] = AVANZAR;
    movs[numMov] = distance;
    return distance;
}

int buscarSalida(double* distancias, int length){
    int ancho_robot = calcularAnchoRobot(distancias, length);
    cout<<"ancho_robot: "<<ancho_robot<<endl;
    int indexDirection = -1;
    int indexDirectionTemp = 0;
    int indexlastNegativeValue;
    double salida, sum, mejorSalida;
    salida = 1000;
    mejorSalida = 1000;
    while(indexDirectionTemp < length - ancho_robot){
        //cout<<indexDirectionTemp<<": "<<distancias[indexDirectionTemp]<<"\n";
        if(distancias[indexDirectionTemp] >= 0){
            //estos son los valores que me interesan. Los valores 
            //negativos me indican que no tengo informacion disponible de ese punto.
            indexlastNegativeValue = -1;
            sum = 0.0;
            for(int k = indexDirectionTemp; k < (indexDirectionTemp + ancho_robot); k++ ){
                //cout<<"k: "<<k<<endl;
                if(distancias[k] < 0){
                    indexlastNegativeValue = k + 1;
                }
                sum += distancias[k];
            }
            //cout<<"indexlastNegativeValue: "<<indexlastNegativeValue<<endl;
            if(indexlastNegativeValue >= 0){
                //algun valor del subarreglo del tamaño del robot es negativo
                //(es decir no tengo informacion del punto)
                indexDirectionTemp = indexlastNegativeValue;
            }
            else{
                //los valores del subarreglo del tamaño del robot son todos positivos
                //(es decir tengo informacion del punto)
                salida =  (sum / (double)ancho_robot);
                //cout<<salida<<endl;
                if(salida < mejorSalida){
                    mejorSalida = salida;
                    indexDirection = indexDirectionTemp;
                }
                indexDirectionTemp++;
            }
        }
        else{
            indexDirectionTemp++;
        }
    }
    cout<<"indexDirection: "<<indexDirection<<" de "<<length<<endl;
    cout<<"mejor salida(distancia mas lejana): "<<mejorSalida<<endl;
    calcularAnguloGiro(indexDirection);
    
    double max = -1;
    for(int i = 0; i < length; i++){
        if(distancias[i] > max){
            max = distancias[i];
        }
    }
    
    calcularDistanciaArecorrer(mejorSalida, max);
	return indexDirection;
}

void navegacion(Mat& disparityMap){
    cv::Mat roiTemp, roi;
    roi = disparityMap.rowRange(340,380);
    //roi = roiTemp.colRange(0,640);
    
    Size size = roi.size();
    
    cout<<"ancho: "<<size.width<<endl;
    cout<<"alto: "<<size.height<<endl;
    
    double distancias [size.width];
    
    vectorDistancia(roi, distancias);
    
    /*for(int i = 0; i < size.width; i++ ){
        cout<<i<<": "<<distancias[i]<<"\n";
    }*/
    
    int dir = buscarSalida(distancias, size.width);

	//pongamos una linea por donde iria

    cv::Mat_<Vec3b> roiColor (roi.size());
	map2Color(roi, roiColor);

	cv::line(roiColor, cv::Point2f((float) dir, 0.0), cv::Point2f((float) dir, 100.0), CV_RGB(255, 255, 255));	
    
    cv::namedWindow("imagencolor1", CV_WINDOW_AUTOSIZE);
    cv::imshow("imagencolor1",roiColor);

    cv::waitKey(0);
}

void calcularMapa(){
    //calcula el mapa explorado(hasta el momento) del lugar
    
}

int main(int argc, char *argv[]) {
    //le carga los parámetros intrinsecos de la camara
    string parameterFileName = "parameters.xml";
    
    cv::FileStorage fs(parameterFileName, cv::FileStorage::READ);
	
	if (!fs.isOpened()){
        cout<<"Error"<<endl;
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
    
    //Imagenes capturadas
    cv::Mat_<Vec3b> img_izq, img_der;
    
    cv::Size size = Size(640,480);
    
    //Matrices ya rectificadas
    cv::Mat P1, P2, R1, R2, Q;
    double alpha = 0.0;
    cv::Rect roi1, roi2;
    
    //MAPA DE DISPARIDAD
    cv::Mat_<float> dispMap_left(size);
    cv::Mat_<float> dispMap_right(size);
    
    //INFO RECTIFICACION
    cv::stereoRectify(p1, dist1, p2, dist2, size, r, t, R1, R2, P1, P2, Q, alpha, size, &roi1, &roi2, cv::CALIB_ZERO_DISPARITY); 

    cv::Mat mapx1, mapy1, mapx2, mapy2;
    cv::initUndistortRectifyMap(p1, dist1, R1, P1, size, CV_32FC1, mapx1, mapy1);
    cv::initUndistortRectifyMap(p2, dist2, R2, P2, size, CV_32FC1, mapx2, mapy2);
    
    tipoMovs = new int[100];
    movs = new double [100];
    
    //Empieza el ciclo que realiza el proceso de navegación
    //Ahora es un for pero despues cambiara.
    //for(int i = 0; i < 100; i++){
        //sacamos las fotos
        tomarImagenes(img_izq, img_der);
        //capturarImagenesDesdeVideo(img_izq, img_der, 100);

        cout<<"hola"<<endl;
        disparity(img_izq, img_der, mapx1, mapy1, mapx2, mapy2, dispMap_left, dispMap_right);    
		
		cv::namedWindow("dispMapleft", CV_WINDOW_AUTOSIZE);
	    cv::imshow("dispMapleft", dispMap_left);
		
		navegacion(dispMap_left);
		
		cv::waitKey(0);
		cout<<"chau"<<endl;

    //}
}

/*int main(int argc, char *argv[]) {

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
}*/

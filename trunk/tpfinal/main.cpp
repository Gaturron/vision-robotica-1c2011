#include "vision.h"
//#include "libexabot-remote/libexabot-remote.h"

const int AVANZAR = 0;
const int ANGULO = 1;
int* tipoMovs;
double* movs;
int numMov = 0;
double posActualx = 0;
double posActualy = 0;
cv::Mat Q;

double cantDeSegPorCadaCm = 2.0;
double velocidad = 0.5;
double cantDeSegPorCadaGrado = 0.1;
double relAnguloIndex;

/*********************************************************************
    FUNCIONES PARA MOVER AL ROBOT
**********************************************************************/

bool exa_remote_initialize(const char* hostname){
    cout<<"inicializo libexabot"<<endl;
    return true;
}

void exa_remote_deinitialize(void){
    cout<<"desinicializo libexabot"<<endl;
}
  
void exa_remote_set_motors(float left, float right){
    cout<<"muevo el robotito"<<endl;
}

int initRobotConf() {
cout <<"+" << "initRobotConf " << endl;
	string confFileName="conf.xml";

	cv::FileStorage fsconf(confFileName, cv::FileStorage::READ);
	
	if (!fsconf.isOpened()){
        cout<<"Error"<<endl;
        return 1;
	}    
    
  cv::FileNode rootConf = fsconf.root();
  cv::Mat mat;
  rootConf["velocidadDesplazamiento"] >> mat;
  double velocidadDesplazamiento = mat.at<double>(0,0);
    cantDeSegPorCadaCm = velocidadDesplazamiento;

  rootConf["velocidadMotor"] >> mat;
  double velocidadMotor = mat.at<double>(0,0);
	velocidad = velocidadMotor;

  rootConf["velocidadGiro"] >> mat;
  double velocidadGiro = mat.at<double>(0,0);
	cantDeSegPorCadaGrado = velocidadGiro;

  rootConf["relAngIndex"] >> mat;
  relAnguloIndex = mat.at<double>(0,0);
}

void desplazarse(double distancia, int direccion){
cout <<"+" << "desplazar " << distancia * direccion << endl;
  double tiempo = cantDeSegPorCadaCm * distancia;  
  /*while (tiempo > 0) {
    double intensidad = velocidad * direccion;
    exa_remote_set_motors(intensidad, intensidad);
    sleep(1);  
    tiempo --;
  }*/    
  exa_remote_set_motors(0, 0);
}

void girar(double angulo){
  cout <<"+" << "girar " << angulo << endl;
  double tiempo = cantDeSegPorCadaGrado * angulo;  
	/*while (tiempo > 0) {
    if(angulo > 0){
      exa_remote_set_motors(-1 * velocidad, velocidad);
    }else{
      exa_remote_set_motors(velocidad, -1 * velocidad);
    }
    sleep(1);  
    tiempo --;
  }*/     
  exa_remote_set_motors(0, 0);
}

void pruebaRobot(){
	//hacemos unos movimientos para ver si los parametros andan
	//	El recorrido es:
	//1) Girar 90 grados a la derecha
	girar(90);	
	//2) Girar 180 grados a la izquierda
	girar(-180);
	//3) desplazarse 20 cm adelante
	desplazarse(20, 1);
	//4) desplazarse 10 cm para atras
	desplazarse(10, -1);
}

void configurarParametros(){
    double valor_avanzar, avance_robot, cantDeSegPorCadaCm_nueva, valor_girar, giro_robot, cantDeSegPorCadaGrado_nueva;
    //configura los parametros del robot
    //CONFIGURO LA VELOCIDAD DE DESPLAZAMIENTO DEL ROBOT
    cout<<"indique la cantidad de centimetros que desea que el robot se mueva hacia adelante: ";
    cin>>valor_avanzar;
    cout<<"indique cuanto avanzo el robot para adelante: ";
    cin>>avance_robot;
    cantDeSegPorCadaCm_nueva = (cantDeSegPorCadaCm * valor_avanzar)/avance_robot;
    //CONFIGURO LA VELOCIDAD DE GIRO DEL ROBOT
    cout<<"indique la cantidad de grados que desea que el robot gire: ";
    cin>>valor_girar;
    cout<<"indique cuanto giro el robot: ";
    cin>>giro_robot;
    cantDeSegPorCadaGrado_nueva = (cantDeSegPorCadaGrado * valor_girar)/giro_robot;
    cout<<"VELOCIDAD DE DESPLAZAMIENTO: "<<cantDeSegPorCadaCm_nueva<<endl;
    cout<<"VELOCIDAD DE GIRO: "<<cantDeSegPorCadaGrado_nueva<<endl;
}

void casoDeTest(){
    //nota ver mapa asociado
}

/*********************************************************************
    FUNCIONES PARA DECIDIR DONDE VA EL ROBOT
**********************************************************************/

void tomarImagenes(Mat& img_izq, Mat& img_der){
    VideoCapture cap1, cap2;
    
    cv::Mat_<Vec3b> frame_izq, frame_der;
    
    frame_der = cv::imread("/home/aolmedo/imgtpfinal/rightcam/rightcam_460.tiff",1);
    frame_izq = cv::imread("/home/aolmedo/imgtpfinal/leftcam/leftcam_460.tiff",1);
    
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
        girar(90);
    }else{
        int temp = indexDirection - 320;
        relAnguloIndex = (double) 90.0/640.0;    
        angulo = temp * relAnguloIndex;    
    }
    cout<<"angulo: "<<angulo<<endl;
    tipoMovs[numMov] = ANGULO;
    movs[numMov] = angulo;
    return angulo;
}

double calcularDistanciaArecorrer(double mejorSalida, double max, int index){
    //tenemos que transformar el valor de disparidad en distancia
    //(igual no sabemos como se le indica la distancia a recorrer al robot)
    double X, Y, Z, W;
    double x, y, d;
    
    cv::Size size = Size(1,4);
    cv::Mat_<double> L(size);
    cv::Mat_<double> punto3D(size);
    
    double distance;
    double coeficiente = (mejorSalida/max);
    
    if(index > 0){
        x = index;
        y = 380;
        
        L.at<double>(0,0) = x;
        L.at<double>(0,1) = y;
        L.at<double>(0,2) = mejorSalida;
        L.at<double>(0,3) = 1;
        
        punto3D = Q * L;
        
        X = punto3D.at<double>(0,0)/punto3D.at<double>(0,3);
        Y = punto3D.at<double>(0,1)/punto3D.at<double>(0,3);
        Z = punto3D.at<double>(0,2)/punto3D.at<double>(0,3);
        W = punto3D.at<double>(0,3)/punto3D.at<double>(0,3);
        
        cout<<"X: "<<X/1000<<" cm"<<endl;
        cout<<"Y: "<<Y/1000<<" cm"<<endl;
        cout<<"Z: "<<Z/1000<<" cm"<<endl;
        
        if( coeficiente < 0.01){
            coeficiente = 0.01;
        }
        distance = 10 / coeficiente;
        cout<<"distancia: "<<distance<<endl;
        tipoMovs[numMov] = AVANZAR;
        movs[numMov] = distance;
    }
    else{
        //politica de escape
        girar(90);
    }
    distance = Z/1000;
    return distance;
}

int buscarSalida(double* distancias, int length, double& angulo, double& distancia){
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
    angulo = calcularAnguloGiro(indexDirection);
    
    double max = -1;
    for(int i = 0; i < length; i++){
        if(distancias[i] > max){
            max = distancias[i];
        }
    }
    
    distancia = calcularDistanciaArecorrer(mejorSalida, max, indexDirection);
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
    
    double angulo, distancia;
    
    int dir = buscarSalida(distancias, size.width, angulo, distancia);
    
    girar(angulo);
    if(distancia > 0){
        desplazarse(distancia, 1);
    }else{
        desplazarse(-distancia, -1);
    }
    
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
    string confFileName="conf.xml";
    
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
    
    
    /*cv::FileStorage fsconf(confFileName, cv::FileStorage::READ);
	
	if (!fsconf.isOpened()){
        cout<<"Error"<<endl;
        return 1;
	}    
    
    cv::FileNode rootConf = fsconf.root();
    cv::Mat mat;
    rootConf["velocidadDesplazamiento"] >> mat;
    double velocidadDesplazamiento = mat.at<double>(0,0);
    rootConf["velocidadMotor"] >> mat;
    double velocidadMotor = mat.at<double>(0,0);
    rootConf["velocidadGiro"] >> mat;
    double velocidadGiro = mat.at<double>(0,0);
    rootConf["relAngIndex"] >> mat;
    relAnguloIndex = mat.at<double>(0,0);
    
    cout<<"relAngIndex:"<<relAnguloIndex<<endl;*/
    
    initRobotConf();
    
    //Imagenes capturadas
    cv::Mat_<Vec3b> img_izq, img_der;
    
    cv::Size size = Size(640,480);
    
    //Matrices ya rectificadas
    cv::Mat P1, P2, R1, R2;
    double alpha = 0.0;
    cv::Rect roi1, roi2;
    
    //MAPA DE DISPARIDAD
    cv::Mat_<float> dispMap_left(size);
    cv::Mat_<float> dispMap_right(size);
    
    //INFO RECTIFICACION
    cv::stereoRectify(p1, dist1, p2, dist2, size, r, t, R1, R2, P1, P2, Q, alpha, size, &roi1, &roi2, cv::CALIB_ZERO_DISPARITY); 
    
    for(int x = 0; x < Q.cols; x++){
        for(int y = 0; y < Q.rows; y++){
            cout<<" "<< Q.at<double>(x,y);
        }
        cout<<endl;
    }
    
    cv::Mat mapx1, mapy1, mapx2, mapy2;
    cv::initUndistortRectifyMap(p1, dist1, R1, P1, size, CV_32FC1, mapx1, mapy1);
    cv::initUndistortRectifyMap(p2, dist2, R2, P2, size, CV_32FC1, mapx2, mapy2);
    
    tipoMovs = new int[100];
    movs = new double [100];
    
    exa_remote_initialize("192.168.0.2");
    
    //configurarParametros();
    
    //Empieza el ciclo que realiza el proceso de navegación
    //Ahora es un for pero despues cambiara.
    for(int i = 0; i < 1; i++){
        //sacamos las fotos
        tomarImagenes(img_izq, img_der);
        //capturarImagenesDesdeVideo(img_izq, img_der, 100);

        cout<<"hola"<<endl;
        disparity(img_izq, img_der, mapx1, mapy1, mapx2, mapy2, dispMap_left, dispMap_right);    
		
        cv::Mat_<Vec3b> colorDispMap_left (dispMap_left.size());
        map2Color(dispMap_left, colorDispMap_left);
        
		cv::namedWindow("dispMapleft", CV_WINDOW_AUTOSIZE);
	    cv::imshow("dispMapleft", colorDispMap_left);
		
		navegacion(dispMap_left);
		
		cv::waitKey(0);
		cout<<"chau"<<endl;

    }
    exa_remote_deinitialize();
}

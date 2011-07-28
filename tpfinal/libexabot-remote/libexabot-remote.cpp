#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include "libexabot-remote.h"
using namespace std;
using boost::asio::ip::udp;

udp::socket* s;
udp::endpoint receiver_endpoint;
boost::asio::io_service io_service;

double cantDeSegPorCadaCm = 2.0;
double velocidad = 0.5;
double cantDeSegPorCadaGrado = 0.1;

extern "C"
bool exa_remote_initialize(const char* hostname)
{
  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), hostname, "7654");  
  receiver_endpoint = *resolver.resolve(query);
  
  s = new udp::socket(io_service);
  s->open(udp::v4());
  return true;
}

void exa_remote_deinitialize(void) {
  delete s;
  s = NULL;
}

void exa_remote_set_motors(float left, float right) {
  if (left > 1) left = 1; else if (left < -1) left = -1;
  if (right > 1) right = 1; else if (right < -1) right = -1;
  
  char cmd[3];
  cmd[0] = 0x3;
  cmd[1] = (char)roundf(right * 30);
  cmd[2] = (char)roundf(left * 30);
  s->send_to(boost::asio::buffer(cmd, 3), receiver_endpoint);
}

void initRobotConf() {
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
  double relAnguloIndex = mat.at<double>(0,0);
}

void deplazarse(double distancia, int direccion){
  double tiempo = cantDeSegPorCadaCm * distancia;  
  while (tiempo > 0) {
    double intensidad = velocidad * direccion;
    exa_remote_set_motors(intensidad, intensidad);
    sleep(1);  
    tiempo --;
  }     
  exa_remote_set_motors(0, 0);
}

void girar(double angulo){
  double tiempo = cantDeSegPorCadaGrado * angulo;  
	while (tiempo > 0) {
    if(angulo > 0){
      exa_remote_set_motors(-1 * velocidad, velocidad);
    }else{
      exa_remote_set_motors(velocidad, -1 * velocidad);
    }
    sleep(1);  
    tiempo --;
  }     
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

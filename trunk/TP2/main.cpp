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
   
   
   cv::MatIterator_<double> myIter = p1.begin<double>(), it_end = p1.end<double>();
   
   for (; myIter!=it_end; ++myIter){
    double v = *myIter;
    cout << v << endl;
   } 
   return 0;
}

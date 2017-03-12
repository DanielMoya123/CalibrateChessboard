#include "YmlRW.h"

// The nameSpaces
using namespace cv;
using namespace std;

/******************************************************
 * 
 *  The constructor
 * 
 * **********************************************/
YmlRW::YmlRW(){};


/******************************************************
 * 
 *  Function to read the yml file
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		-------
 * 
 * Outputs:
 * 		vector<Mat> outputPoints: [0] = cameraMatrix & [1] = distCoeffs
 * 			Mat cameraMatrix: The matrix of the camera calibration matrix
 * 			Mat distCoeffs: The matrix of the coefficients of distances	
 * 
 * Restrinctions:
 * 		-------
 * 
 *****/
vector<Mat> YmlRW::ReadYAML(){
  
	vector<Mat> outputMatrices;
  
	// We create a file with the name calibrate Yml
	FileStorage fs2("calibrate.yml", FileStorage::READ);
	
	// Why try to open the file
	if(fs2.isOpened()){
	 
		// We take the data
		Mat cameraMatrix, distCoeffs;
		fs2["cameraMatrix"] >> cameraMatrix;
		fs2["distCoeffs"] >> distCoeffs;
		fs2.release();

		// Set the data into the output matrix
		outputMatrices.push_back(cameraMatrix);
		outputMatrices.push_back(distCoeffs);
	}

	return outputMatrices;
	
}

/******************************************************
 * 
 *  Function to write the yml file
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		Mat cameraMatrix: The matrix of the camera calibration matrix
 * 		Mat distCoeffs: The matrix of the coefficients of distances	
 * 
 * Outputs:
 * 		-------
 * 
 * Restrinctions:
 * 		-------
 * 
 *****/
void YmlRW::WriteYAML(Mat cameraMatrix, Mat distCoeffs){ 

	// We generate the file and write the values
	FileStorage fs("calibrate.yml", FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();
    
}

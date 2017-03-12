#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

#include "Camera.h"
#include "YmlRW.h"

using namespace cv;
using namespace std;


/******************************************************
 * 
 * Help Function
 * 
 * **********************************************/
void help()  {
  std::cout <<
    "usage: viewer [options] \n" << std::endl;  
  std::cout <<
    "       -c|--calibrate   take pictures to calibrate" << std::endl;   
  std::cout <<
    "       -r|--run         run the augmented reality" << std::endl;  
  std::cout << 
    "       -h|--help        show this help\n"<< std::endl;    
}




/******************************************************
 * 
 * The main function
 * 
 * **********************************************/
int main(int argc, char *argv[])
{

   if (argc > 1) {

    if (std::string(argv[1])=="-h" || std::string(argv[1])=="--help") {
       help();
       return EXIT_SUCCESS;
    }
    
    // Init the other objects
    Camera camera = Camera();
	YmlRW ymlRW = YmlRW();
	
	if (std::string(argv[1])=="-c" || std::string(argv[1])=="--calibrate" ){
		
		VideoCapture stream(0);
		namedWindow("video",1);
	
		vector<string> imageList;
		// The num that represent 'y'
		int r_int = 1048697, num = 0;

		// Ends when the user press a key different than y
		while (r_int == 1048697 || r_int == -1)
		{
			
			// Get the string of the the number
			stringstream ss;
			ss << num;
			string strNum = ss.str();
      
			// Get the matrix of the image
			Mat save_img;
			stream >> save_img;

			// We show the image
			imshow("video", save_img);

			if(save_img.empty())
			{
				std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
				return EXIT_FAILURE;
			}

			// We wait for the keypress
			r_int = waitKey(30);
			
			// Ask if the key was 'y'
			if (r_int == 1048697) {
				
				// Save the image
				string str = "test" + strNum + ".jpg";
				imwrite(str.c_str(), save_img);
				num++;
				imageList.push_back(str);
				cout << "Do you want to take another picture for calibration?[y/n]" << endl;
			}
		}
		
		vector<Mat> matricesResponse = camera.CalibrateImages(imageList);
		ymlRW.WriteYAML(matricesResponse[0],matricesResponse[1]);
		
		return EXIT_SUCCESS;
	}
	
	if (std::string(argv[1])=="-r" || std::string(argv[1])=="--run" ){

		// Read the file with the values
		vector<Mat> matricesResponse = ymlRW.ReadYAML();
		
		vector<vector <Point3f> > objectPoints; 
		vector<vector <Point2f> > imagePoints;
		vector<Mat> rvecs,tvecs;
		
		
		calibrateCamera(objectPoints, imagePoints, camera.imageSize, matricesResponse[0], matricesResponse[1], rvecs, tvecs);
		cout << "aqui2" << endl;

		VideoCapture stream(0);
		namedWindow("video",1);

		while (true)
		{

			Mat imageLines; 
			stream >> imageLines;


			if(imageLines.empty())
			{
				std::cerr << "Something is wrong with the webcam, could not get frame.(2)" << std::endl;
				return EXIT_FAILURE;
			}
			
			vector<Point2f> outputPoints = camera.GetPointAxes(imageLines, matricesResponse[0], matricesResponse[1]);
			
			if(!outputPoints.empty()) {
				line(imageLines, outputPoints[0], outputPoints[3], Scalar(255,  0,   0),2);
				line(imageLines, outputPoints[0], outputPoints[27], Scalar(0,  255,   0),2);
				line(imageLines, outputPoints[0], outputPoints[outputPoints.size()-1], Scalar(0,    0, 255),2);
			}

			imshow("video", imageLines);
			waitKey(30);
			
		}
		
	}
	
  } else {
 
    std::cout << "Provide at leat one function. \n" << std::endl;   
    return EXIT_FAILURE;
    
  }

  return EXIT_SUCCESS;
}

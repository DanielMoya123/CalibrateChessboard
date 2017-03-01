#include <stdlib.h>
#include <stdio.h>

#include <opencv/cv.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <string.h>
#include <cctype>

using namespace cv;
using namespace std;

// Function to read the yml file
void readYAML(Mat cameraMatrix, Mat distCoeffs){
  
	FileStorage fs2("calibrate.yml", FileStorage::READ);
	Mat cameraMatrix2, distCoeffs2;
	fs2["cameraMatrix"] >> cameraMatrix2;
	fs2["distCoeffs"] >> distCoeffs2;
	cameraMatrix = cameraMatrix2;
	distCoeff = distCoeff2;
	
	fs2.release();

	cout << "Ready to draw axes..." << endl;
	drawAxes(corners, imgpts);
  
}

// Function to write the yml file
void writeYAML(const Mat& cameraMatrix, const Mat& distCoeffs){ 
  
	FileStorage fs("calibrate.yml", FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();
    return 0;
  
  }



/*
*Calibrate all the images that we are stored in disc.
* 
* Inputs:
* 	vector<string> imageList: the path to the image
*   int size: the size of the image 
*/
int calibrateImages(vector<string> imageList, int size)
{
	bool found;
	Mat view;
	vector<Point2f> pointbuf;
	Size boardSize(7,6);
	int numSquares = boardSize.height * boardSize.width; // The number of squares
	vector<vector<Point3f>> objectPoints; 
	vector<vector<Point2f>> imagePoints;  

	// Iterate over all images
	for (int i = 0; i < size; i++){
	
		// Generate the matrix for this image
		view = imread(imageList[i], 1);
		found = findChessboardCorners(view, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH);
		vector<Point3f> obj;
		for(int j=0;j<numSquares;j++) 
			obj.push_back(Point3f(j/boardSize.width, j%boardSize.width, 0.0f));
			
		imagePoints.push_back(pointbuf);
		objectPoints.push_back(obj);

	}
	
	
    //imageSize – Image size in pixels used to initialize the principal point.
    Size imageSize(800,600);
    Mat cameraMatrix =  initCameraMatrix2D(objectPoints, imagePoints, imageSize);

	
	//Mat cameraMatrix = Mat(3, 3, CV_32FC1); 
	Mat distCoeffs; 
	vector<Mat> rvecs; 
	vector<Mat> tvecs;
	
	//We modify the intrinsic matrix with whatever we do know.
	//The camera's aspect ratio is 1 (that's usually the case... If not, change it as required).

    //cameraMatrix.ptr<float>(0)[0] = 1;
    //cameraMatrix.ptr<float>(1)[1] = 1;

    double result = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    cout << "Camera Matriz and Dist-coeffs calculated" << endl;
    writeYAML(cameraMatrix, distCoeffs);
    cout << "Camera Matriz and Dist-coeffs saved" << endl;


	//////Parte de otra funcion

    bool temp2 =  solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvecs, tvecs);
    
    vector<Point2f> outputImagePoints;

    projectPoints(objectPoints, rvecs, tvecs, cameraMatrix, distCoeffs, outputImagePoints);

  
}


/*
Tomar todas las fotos que el usuario desee para usarlas posteriormente en la calibración
*/
void takeImage(string Num)
{

	// Take the photo
	VideoCapture capture = VideoCapture(0);
  
	// Get the frame
	Mat save_img; 
	capture >> save_img; //cambio de cap a capture

	if(save_img.empty())
	{
		std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
	}
	
	// Save the frame into a file
	imwrite("test" + Num + "jpg", save_img); // A JPG FILE IS BEING SAVED
  
}



/*
 Tomar los parametros en formato YAML y dibujar los ejes en tiempo real
*/
void drawAxes(corners, imgpts)
{
  
    CvCapture* capture = cvCaptureFromFile( "MOV.MPG" );
    IplImage* frame = cvQueryFrame( capture );
         
    // Can't get device? Complain and quit  
    if( !capture )  
    {  
        std::cout << "Could not initialize capturing...\n";   
    }  
     
    while (true)
    {
        frame = cvQueryFrame(capture); 
         
        if( !frame ) break; 
 
        //Draw the lines     
        Mat imageLines = frameMat.clone(); 
         
        arrowedLine(imageLines, corners[0], imgpts[0], Scalar(255,  0,   0));
		arrowedLine(imageLines, corners[0], imgpts[1], Scalar(0,  255,   0));
		arrowedLine(imageLines, corners[0], imgpts[2], Scalar(0,    0, 255));
             
        imshow("video", contourImage);
        cvWaitKey(40);
    }
 
    // We're through with using camera.   
    cvReleaseCapture( &capture );  
 
    return 0;  
}




int main(int argc, char *argv[])
{

   if (argc > 1) {

    if (std::string(argv[1])=="-h" || std::string(argv[1])=="--help") {
       help();
       return EXIT_SUCCESS;
    }
	
	if (std::string(argv[1])=="-c" || std::string(argv[1])=="--calibrate" ){
	
		cout << "Taking pictures for future calibration..." << endl;
		vector<string> imageList;
		int num = 0;
		char r = 'n';
		
		while (r != 'n')
		{
		  takeImage(num);
		  num++;
		  imageList.push_back("test"+std::to_string(num));
		  cout << "Do you want to take another picture for calibration?[y/n]" << endl;
		  cin >> r;
		}

		cout << "Beginning camera calibration..." << endl;
		calibrateImages(imageList, num);
    
	}

	Mat cameraMatrix, distCoeffs;

    cout << "Reading camera parameters..." << endl;
    readYAML(cameraMatrix,distCoeffs);

	

  } else {
	  
    std::cout << "Provide at least one image file to be displayed. \n" 
              << "Usage: " << argv[0] << " <image1>"
              << std::endl;
              
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

/*
 * Help Function
 */
void help()  {
  std::cout <<
    "usage: viewer [options] \n\n" \
    "       -c|--calibrate   take pictures to calibrate\n" \   
    "       -h|--help        show this help\n"<< std::endl;    
}





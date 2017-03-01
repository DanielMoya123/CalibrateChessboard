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



void readYAML(){
  
  FileStorage fs2("test.yml", FileStorage::READ);

// first method: use (type) operator on FileNode.
int frameCount = (int)fs2["frameCount"];

std::string date;
// second method: use FileNode::operator >>
fs2["calibrationDate"] >> date;

Mat cameraMatrix2, distCoeffs2;
fs2["cameraMatrix"] >> cameraMatrix2;
fs2["distCoeffs"] >> distCoeffs2;

cout << "frameCount: " << frameCount << endl
     << "calibration date: " << date << endl
     << "camera matrix: " << cameraMatrix2 << endl
     << "distortion coeffs: " << distCoeffs2 << endl;

FileNode features = fs2["features"];
FileNodeIterator it = features.begin(), it_end = features.end();
int idx = 0;
std::vector<uchar> lbpval;

// iterate through a sequence using FileNodeIterator
for( ; it != it_end; ++it, idx++ )
{
    cout << "feature #" << idx << ": ";
    cout << "x=" << (int)(*it)["x"] << ", y=" << (int)(*it)["y"] << ", lbp: (";
    // you can also easily read numerical arrays using FileNode >> std::vector operator.
    (*it)["lbp"] >> lbpval;
    for( int i = 0; i < (int)lbpval.size(); i++ )
        cout << " " << (int)lbpval[i];
    cout << ")" << endl;
}
fs.release();

// corners = pointbuf de la funcion findChessboardCorners
// imgpts = outputImagePoints de la funcion projectPoints
cout << "Ready to draw axes..." << endl;
drawAxes(corners, imgpts);
  
}

void writeYAML(const Mat& cameraMatrix, const Mat& distCoeffs){ //Se agregan parámetros de entrada
  
  FileStorage fs("test.yml", FileStorage::WRITE);

    fs << "frameCount" << 5;
    time_t rawtime; time(&rawtime);
    fs << "calibrationDate" << asctime(localtime(&rawtime));
    //Mat cameraMatrix = (Mat_<double>(3,3) << 1000, 0, 320, 0, 1000, 240, 0, 0, 1);
    //Mat distCoeffs = (Mat_<double>(5,1) << 0.1, 0.01, -0.001, 0, 0);
    fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs << "features" << "[";
    for( int i = 0; i < 3; i++ )
    {
        int x = rand() % 640;
        int y = rand() % 480;
        uchar lbp = rand() % 256;

        fs << "{:" << "x" << x << "y" << y << "lbp" << "[:";
        for( int j = 0; j < 8; j++ )
            fs << ((lbp >> j) & 1);
        fs << "]" << "}";
    }
    fs << "]";
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
  Size boardSize(7,6); //Interior number of corners (cambio para que quede impar,par)
  int numSquares = boardSize.height * boardSize.width; // The number of squares
  vector<vector<Point3f>> objectPoints; 
  vector<vector<Point2f>> imagePoints;  

  // Iterate over all images
  for (int i = 0; i < size; i++){
	
	// Generate the matrix for this image
    view = imread(imageList[i], 1);
	 
    found = findChessboardCorners(view, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH);

    //imageSize – Image size in pixels used to initialize the principal point.
    Size imageSize(800,600);
    
    vector<Point3f> obj;
    
    
    for(int j=0;j<numSquares;j++) 
		obj.push_back(Point3f(j/boardSize.width, j%boardSize.width, 0.0f));
		
	imagePoints.push_back(pointbuf);
	objectPoints.push_back(obj);

	}

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

    writeYAML(cameraMatrix, distCoeffs)

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

  bool endCalibration = false;
  vector<string> imageList


   if (argc > 1) {

    if (std::string(argv[1])=="-h") {
       help();
       return EXIT_SUCCESS;
    }

    cout << "Taking pictures for future calibration..." << endl;
    int num = 0;
    while (endCalibration == false)
    {
      takeImage(num);
      num++;
      cout << "Do you want to take another picture for calibration?[y/n]" << endl;
      char r;
      cin >> r;
      if (r == 'n')
        endCalibration = true;
    }

    for (int i = 0; i < num, i++) {
      imageList.push_back("test"+i); // no se si puede realizar la operacion por ser string + int
    }

    cout << "Beginning camera calibration..." << endl;

    calibrateImages(imageList, num);


    cout << "Reading camera parameters..." << endl;
    readYAML();




    

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
    "usage: viewer [options] [<file1>]\n\n"        \
    "       -h|--help        show this help\n"<< std::endl;    
}





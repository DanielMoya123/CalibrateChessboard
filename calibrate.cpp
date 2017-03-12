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

/***************************************
 * Constant Values
 ***************************************/

const Size boardSize(9,6);
const Size imageSize(640,480);




vector<Point2f> GetPointAxes(Mat imageList, Mat cameraMatrix, Mat distCoeffs)
{
  
  bool found;
  //Mat view;
  vector<Point2f> pointbuf;
  vector<Point2f> outputImagePoints;
  
  int numSquares = boardSize.height * boardSize.width; // The number of squares
  //vector<vector <Point3f> > objectPoints; 
  //vector<vector <Point2f> > imagePoints;  
  //cout << "in1" << endl;
  
  // Generate the matrix for this image
  //view = imageList;
  found = findChessboardCorners(imageList, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK);
  vector<Point3f> obj;
  //obj.push_back(Point3f(0, 0, 1.0f));

  if (found){



    vector<Point3f> z3d;
    //cout << found << endl;
    for(int j=0;j<numSquares;j++) {
      obj.push_back(Point3f(j/boardSize.width, j%boardSize.width, 0.0f));
    }
        
    z3d.push_back(Point3f(0.0f,0.0f,3.0f));
    
    //objectPoints.push_back(obj);

          //Mat imageLines(frame); 

          //cout << "tam cam " << cameraMatrix.size() << endl;
          //cout << "tam dist" << distCoeff

    //cout << "in3" << endl;
    Mat rvecs;

    Mat tvecs;


      bool temp2 =  solvePnP(obj, pointbuf, cameraMatrix, distCoeffs, rvecs, tvecs);
      //cout << "in4" << endl;
      
      vector<Point2f> outputImagePoints2;
      projectPoints(obj, rvecs, tvecs, cameraMatrix, distCoeffs, outputImagePoints);
      projectPoints(z3d, rvecs, tvecs, cameraMatrix, distCoeffs, outputImagePoints2);

      outputImagePoints.push_back(outputImagePoints2[0]);
    }

    
    return outputImagePoints;
}

/*
 Tomar los parametros en formato YAML y dibujar los ejes en tiempo real
*/
void drawAxes(Mat cameraMatrix, Mat distCoeffs)
{

    //cout << "entra" << endl; 
  
    //CvCapture* capture = cvCaptureFromCAM(0);
    //IplImage* frame = cvQueryFrame( capture );
    VideoCapture stream(0);
    namedWindow("video",1);
     


  
    //cout << "entra2" << endl; 

    // Can't get device? Complain and quit  
    /*if( !capture )  
    {  
        std::cout << "Could not initialize capturing...\n";   
    }  */
     
    while (true)
    {
        //cout << "entra3" << endl;
        //frame = cvQueryFrame(capture); 
         
        //if( !frame ) break; 



        // Get the frame
        Mat imageLines; 
        stream >> imageLines; //cambio de cap a capture


        if(imageLines.empty())
        {
          std::cerr << "Something is wrong with the webcam, could not get frame.(2)" << std::endl;
          break;
        }
 
        //Draw the lines     
        //Mat imageLines(frame); 

        //cout << "tam cam " << cameraMatrix.size() << endl;
        //cout << "tam dist" << distCoeffs.size() << endl;
        
        vector<Point2f> outputPoints = GetPointAxes(imageLines, cameraMatrix, distCoeffs);
        //cout << "entra6" << endl;
        //cout << "output tam " << outputPoints.size() << endl;
        if(!outputPoints.empty()) {
          line(imageLines, outputPoints[0], outputPoints[3], Scalar(255,  0,   0), 3);
          line(imageLines, outputPoints[0], outputPoints[27], Scalar(0,  255,   0), 3);
          line(imageLines, outputPoints[0], outputPoints[54], Scalar(0,    0, 255), 3);
        }
        //cout << "entra4" << endl;

        imshow("video", imageLines); //corregido segundo argumento
        waitKey(30);
        //cout << "entra5" << endl;
    }
 
    // We're through with using camera.   
    //cvReleaseCapture( capture );  
 
}



// Function to read the yml file
bool readYAML(Mat cameraMatrix, Mat distCoeffs){
  
	FileStorage fs2("calibrate.yml", FileStorage::READ);
	
	if(fs2.isOpened()){
	 
		Mat cameraMatrix2, distCoeffs2;
		fs2["cameraMatrix"] >> cameraMatrix2;
		fs2["distCoeffs"] >> distCoeffs2;
		cameraMatrix = cameraMatrix2;
		distCoeffs = distCoeffs2;
		
		fs2.release();


  

		cout << "Ready to draw axes..." << endl;

    //vector<Point2f> imgpts = DrawAxis(Mat imageList)

		//drawAxes(corners, imgpts);
    drawAxes(cameraMatrix, distCoeffs);

		return true;
	}else{
		return false;
	}

}

// Function to write the yml file
void writeYAML(const Mat& cameraMatrix, const Mat& distCoeffs){ 

  cout << "tam cam " << cameraMatrix.size() << endl;
  cout << "tam dist" << distCoeffs.size() << endl;
  
	FileStorage fs("calibrate.yml", FileStorage::WRITE);
	fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;
    fs.release();
  
  }



/*
*Calibrate all the images that we are stored in disc.
* 
* Inputs:
* 	vector<string> imageList: the path to the image
*   int size: the size of the image 
*/
int calibrateImages(vector<string> imageList, int cantImg)
{
	bool found;
	Mat view;
	vector<Point2f> pointbuf;
	int numSquares = boardSize.height * boardSize.width; // The number of squares
	vector<vector <Point3f> > objectPoints; 
	vector<vector <Point2f> > imagePoints;  

  vector<Point3f> obj;
  for(int j=0;j<numSquares;j++) 
    {
      obj.push_back(Point3f(j/boardSize.width, j%boardSize.width, 0.0f));
    }

	// Iterate over all images
	for (int i = 0; i < cantImg; i++){
	
		// Generate the matrix for this image
		view = imread(imageList[i], 1);


		found = findChessboardCorners(view, boardSize, pointbuf, CV_CALIB_CB_ADAPTIVE_THRESH);
		
    
		
			
		imagePoints.push_back(pointbuf);
		objectPoints.push_back(obj);


	}

	
	cout << "cant " <<  cantImg << endl;
    //imageSize – Image size in pixels used to initialize the principal point.

  cout << "tamanio obj " << objectPoints.size() << endl;

  cout << "tamanio image" << imagePoints.size() << endl;

  for (int i = 0; i < objectPoints.size(); i++){
      cout << "tamanio obj en " << i << " es " << objectPoints[i].size() << endl;
      cout << "tamanio img en " << i << " es " << imagePoints[i].size() << endl;
  }



    Mat cameraMatrix =  initCameraMatrix2D(objectPoints, imagePoints, imageSize);
  cout << "pasa2" << endl;
	
	//Mat cameraMatrix = Mat(3, 3, CV_32FC1); 
	Mat distCoeffs; 
	vector<Mat> rvecs,tvecs;
	
	//We modify the intrinsic matrix with whatever we do know.
	//The camera's aspect ratio is 1 (that's usually the case... If not, change it as required).

    //cameraMatrix.ptr<float>(0)[0] = 1;
    //cameraMatrix.ptr<float>(1)[1] = 1;

	calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
    cout << "Camera Matriz and Dist-coeffs calculated" << endl;
    writeYAML(cameraMatrix, distCoeffs);
    cout << "Camera Matriz and Dist-coeffs saved" << endl;
  
}









/*
 * Help Function
 */
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




int main(int argc, char *argv[])
{

   if (argc > 1) {

    if (std::string(argv[1])=="-h" || std::string(argv[1])=="--help") {
       help();
       return EXIT_SUCCESS;
    }

    //CvCapture* capture = cvCaptureFromCAM(0);
	   

	if (std::string(argv[1])=="-c" || std::string(argv[1])=="--calibrate" ){
    VideoCapture stream(0);
     namedWindow("video",1);
	
		cout << "Taking pictures for future calibration..." << endl;
		vector<string> imageList;
		int num = 0;
		char r = 'y';
		int r_int = 1048697;


		while (r_int == 1048697 || r_int == -1)
		{
      stringstream ss;
      ss << num;
      string strNum = ss.str();


      //put the code itself of takeImage      
		  //takeImage(str);
      
      //IplImage* frame = cvQueryFrame( capture );
      
      Mat save_img;
      //stream.read(save_img);
      stream >> save_img;

      //cvtColor(save_img, video, CV_BGR2GRAY);
       // GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
       // Canny(edges, edges, 0, 30, 3);

        imshow("video", save_img);
   



      if(save_img.empty())
      {
        std::cerr << "Something is wrong with the webcam, could not get frame." << std::endl;
      }

      // 'n' = 1048686


      r_int = waitKey(30);
      if (r_int == 1048697) { // 'y'
        string str = "test" + strNum + ".jpg";
        const char *imageName = str.c_str();
        imwrite(imageName, save_img); // A JPG FILE IS BEING SAVED
        num++;
        imageList.push_back(str);
        cout << "Do you want to take another picture for calibration?[y/n]" << endl;
      } 

      //cout << "r_int es " << r_int << endl;
		  //cin >> r;
		}

		cout << "Beginning camera calibration..." << endl;

    for (int i = 0; i < num; i++){
      cout << "nombre imagen en " << i << " es " << imageList[i] << endl;
    }

    //vector<Point2f> pointbuf;
		calibrateImages(imageList, num);
		
		return EXIT_SUCCESS;
	}
	
	if (std::string(argv[1])=="-r" || std::string(argv[1])=="--run" ){

		Mat cameraMatrix, distCoeffs;

		cout << "Reading camera parameters..." << endl;
		readYAML(cameraMatrix,distCoeffs);
		
		vector<vector <Point3f> > objectPoints; 
		vector<vector <Point2f> > imagePoints;
		vector<Mat> rvecs,tvecs;
		//calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs); //eliminado?
	   

  
		//DrawAxis(); //eliminado?
	}
	

  } else {
 
    std::cout << "Provide at leat one function. \n" << std::endl;   
    return EXIT_FAILURE;
    
  }

  return EXIT_SUCCESS;
}







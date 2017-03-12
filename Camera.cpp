#include "Camera.h"

// The nameSpaces
using namespace cv;
using namespace std;

/***************************************
 * Constants
 * 
 * 		FLAGS = the flags of the OpenCV functions
 ***************************************/
const int FLAGS = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;


/***************************************
 * Attributes
 * 
 * 		boardSize = the number of inner points in the board
 * 		imageSize = the number of pixels of the image
 * 		numSquares = the multiply of the height and width of the boardSize
 * 		obj = the matrix of the points
 * 		z3d = the point in 3D
 * 
 ***************************************/
Size boardSize,imageSize;
vector<Point3f> obj,z3d;
int numSquares;


/******************************************************
 * 
 * Constructor of the class Camera
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		Size boardSize: The size of the board
 * 		Size imageSize: The size of the image	
 * 
 * Outputs:
 * 		-------
 * 
 * Restrinctions:
 * 		Size boardSize: Use the inner board points, 
 * 						size restricted by the limit of the Size class
 * 						recommended 9,6 inner board points 	
 * 		Size imageSize: The size of the image, 
 * 						size restricted by the limit of the Size class	
 *****/

Camera::Camera(Size boardSize,Size imageSize) {
	
	InitializateInitValues(boardSize,imageSize);
	cout << "The camera object was created" << endl;
}


/******************************************************
 * 
 * Constructor of the class Camera with default values
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		-------
 * 
 * Outputs:
 * 		-------
 * 
 * Restrinctions:
 * 		-------
 *****/
Camera::Camera(void) {
	
	InitializateInitValues(Size(9,6),Size(640,480));
	cout << "The camera object was created with the default values" << endl;
}


/******************************************************
 * 
 * Function to set the init values
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		Size boardSize: The size of the board
 * 		Size imageSize: The size of the image	
 * 
 * Outputs:
 * 		-------
 * 
 * Restrinctions:
 * 		Size boardSize: Use the inner board points, 
 * 						size restricted by the limit of the Size class
 * 						recommended 9,6 inner board points 	
 * 		Size imageSize: The size of the image, 
 * 						size restricted by the limit of the Size class	
 *****/
void Camera::InitializateInitValues(Size boardSize,Size imageSize){
	
	boardSize = boardSize;
	imageSize = imageSize;
	numSquares = boardSize.height * boardSize.width;
    
    // Iterate and fill Obj with all the inner points of the board
	for(int j=0;j<numSquares;j++) {
		obj.push_back(Point3f(j/boardSize.width, j%boardSize.width, 0.0f));
	}
    // Create a point in Z axis 
	z3d.push_back(Point3f(0.0f,0.0f,3.0f));
}

/******************************************************
 * 
 * Function to get the points to draw the axes
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		Mat imageList: The matrix of the image 
 * 		Mat cameraMatrix: The matrix of the camera calibration matrix
 * 		Mat distCoeffs: The matrix of the coefficients of distances		
 * 
 * Outputs:
 * 		Vector<Point2F> outputImagePoints: The vector with points to draw the axes,
 * 											empty if dont find the chessboard
 * 
 * Restrinctions:
 * 		Mat imageList: Has the same size of the camera images of the calibration
 * 		Mat cameraMatrix: It's a matrix 3x3 with the respective values
 * 		Mat distCoeffs: It's a matrix 5x1 with the respective value
 * 
 *****/
vector<Point2f> Camera::GetPointAxes(Mat imageList, Mat cameraMatrix, Mat distCoeffs)
{

	// Ask if the algorithm found a chessboard in the image
	if (findChessboardCorners(imageList, boardSize, pointbuf, FLAGS)){

		// The variables of the functions
		vector<Point2f> pointbuf,z3dImagePoint,outputImagePoints;
		Mat rvecs, tvecs;
		
		// Solve the PnP with the points that we get before
		solvePnP(obj, pointbuf, cameraMatrix, distCoeffs, rvecs, tvecs);
		
		// We proyect the points of the matrix
		projectPoints(obj, rvecs, tvecs, cameraMatrix, distCoeffs, outputImagePoints);
		projectPoints(z3d, rvecs, tvecs, cameraMatrix, distCoeffs, z3dImagePoint);

		// Push the 3d point in the last position
		outputImagePoints.push_back(z3dImagePoint[0]);
    }

    // Return the points
    return outputImagePoints;
}



/******************************************************
 * 
 * Calibrate all the images that we are stored in disc
 * 
 * **********************************************
 * 
 * Inputs: 
 * 		vector<string> imageList: the vector with the paths of the images
 * 
 * Outputs:
 * 		vector<Mat> outputPoints: [0] = cameraMatrix & [1] = distCoeffs
 * 			Mat cameraMatrix: The matrix of the camera calibration matrix
 * 			Mat distCoeffs: The matrix of the coefficients of distances	
 * 
 * Restrinctions:
 * 		vector<string> imageList: dont empty
 * 
 *****/
vector<Mat> Camera::CalibrateImages(vector<string> imageList)
{
	
	// We ask if the list is empty
	if(!imageList.empty()){
		
		// A buffer with the calibration values
		vector<Point2f> pointbuf;
		vector<vector <Point3f> > objectPoints; 
		vector<vector <Point2f> > imagePoints;
		vector<Mat> outputPoints,rvecs,tvecs;
		Mat distCoeffs; 
		
		
		int cantImg = imageList.size();
	
		// Iterate over all images
		for (int i = 0; i < cantImg; i++){
			// Find the chessboardCorners
			findChessboardCorners(imread(imageList[i], 1), boardSize, pointbuf, FLAGS);
			// Push the calibration matrix
			imagePoints.push_back(pointbuf);
			objectPoints.push_back(obj);
		}

		// Calibrate the camera
		Mat cameraMatrix = initCameraMatrix2D(objectPoints, imagePoints, imageSize);
		calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
		
		outputPoints.push_back(cameraMatrix);
		outputPoints.push_back(distCoeffs);
  
	}
}

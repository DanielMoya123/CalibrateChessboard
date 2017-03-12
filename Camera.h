#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

#include <opencv/cv.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;
class Camera
{
	public:
		Camera();
		Camera(Size boardSize,Size imageSize);
		vector<Mat> CalibrateImages(vector<string> imageList);
		vector<Point2f> GetPointAxes(Mat imageList, Mat cameraMatrix, Mat distCoeffs);
		Size imageSize;
	private:
		Size boardSize;
		int numSquares;
		void InitializateInitValues(Size boardSize,Size imageSize);
		const int FLAGS = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CALIB_CB_FAST_CHECK;
		vector<Point3f> obj;
		vector<Point3f> z3d;
};

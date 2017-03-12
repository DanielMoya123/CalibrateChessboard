#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

#include <opencv/cv.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

class Camera
{
	public:
		Camera();
		Camera(Size boardSize,Size imageSize);
		void CalibrateImages(vector<string> imageList);
		vector<Point2f> GetPointAxes(Mat imageList, Mat cameraMatrix, Mat distCoeffs);
	private:
		Size boardSize;
		Size imageSize;
		int numSquares;
		const int FLAGS;
		vector<Point3f> obj;
		vector<Point3f> z3d;
};

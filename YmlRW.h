#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cctype>

class YmlRW
{
	public:
		YmlRW();
		ReadYAML(Mat cameraMatrix, Mat distCoeffs);
		WriteYAML(Mat cameraMatrix,Mat distCoeffs);
};

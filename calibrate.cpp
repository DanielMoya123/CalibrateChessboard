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

/*
Calibrar todas las imagenes que están guardadas en disco.
*/
int calibraImagenes(vector<string> imageList, int size)
{
  bool found;
  Mat view;
  vector<Point2f> pointbuf;


  for (int i = 0; i < size; i++){
    view = imread(imageList[i], 1);

    found = findChessboardCorners( view, boardSize, pointbuf,
                    CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

    //imageSize – Image size in pixels used to initialize the principal point.

    Mat temp =  initCameraMatrix2D(InputArrayOfArrays objectPoints, InputArrayOfArrays imagePoints, Size imageSize);

    double result = calibrateCamera(InputArrayOfArrays objectPoints, InputArrayOfArrays imagePoints, Size imageSize,
       InputOutputArray cameraMatrix, InputOutputArray distCoeffs, OutputArrayOfArrays rvecs, OutputArrayOfArrays tvecs);

    bool temp2 =  solvePnP(InputArray objectPoints, InputArray imagePoints, InputArray cameraMatrix,
     InputArray distCoeffs, OutputArray rvec, OutputArray tvec);

    projectPoints(InputArray objectPoints, InputArray rvec, InputArray tvec, InputArray cameraMatrix, 
      InputArray distCoeffs, OutputArray imagePoints);

    //guardar en formato YAML

    
  }

  
}


/*
Tomar todas las fotos que el usuario desee para usarlas posteriormente en la calibración
*/
void tomaImagenes()
{
  //Toma una fotografía y pregunta si quiere tomar otra.
}

/*
 Tomar los parametros en formato YAML y dibujar los ejes en tiempo real
*/
void dibujaEjes()
{
  //Leer YAML 

  //Funciones para dibujar rectas
}

int main(int argc, char *argv[])
{
  


  return 0;
}





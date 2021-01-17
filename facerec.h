#ifndef FACEREC_H
#define FACEREC_H

#include <iostream>
#include <string>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include <QString>

using namespace std;
using namespace cv;

class FaceRec
{
public:
    FaceRec();
    vector <QString> predictFaces (Mat pic);
    Mat predictedImage ();
};

#endif // FACEREC_H

#define USE_OPENCV_3
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <string>
#include <dirent.h>

#include <cgal.h>
#include <off_screen_renderer.h>

#define TRAIN_RATIO 0.9
#define MODEL_1 "model1.yml"
#define MODEL_2 "model2.yml"
#define MODEL_3 "model3.yml"

using namespace cv::ml;
using namespace cv;
using namespace std;

void trainAllModels (string pathPositive, string pathNegative);
float predict (Mat projection, View view);
float predict (PartBase *part, View view);
int isPlausible (PartBase *part);
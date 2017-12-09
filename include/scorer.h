#define USE_OPENCV_3
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/ml.hpp>
#include <string>
#include <dirent.h>

#define TRAIN_RATIO 0.9
#define MODEL_1 "model1.yml"
#define MODEL_2 "model2.yml"
#define MODEL_3 "model3.yml"

using namespace cv::ml;
using namespace cv;
using namespace std;

enum Model {FRONT, SIDE, TOP};

void trainAllModels (string pathPositive, string pathNegative);
int predict (Mat view, Model model);
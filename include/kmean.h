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

#include <scorer.h>

#define TRAIN_RATIO 0.9
#define MODEL_1 "model1.yml"
#define MODEL_2 "model2.yml"
#define MODEL_3 "model3.yml"

using namespace cv::ml;
using namespace cv;
using namespace std;


void getLabels(vector<PartBase*> chairs);
Mat chairToMat(PartBase *part, View view);
Mat calculateKMeans(vector<Mat> view);
void ConvertVectortoMatrix (vector<vector<float>> &hogSamples,  Mat &samples);
void loadData (vector<Mat> &view1, vector<Mat> &view2, vector<Mat> &view3, string &pathName);
void getHOG(vector<vector<float>> &hogVectors, vector<Mat> &samples);
void printLabels(Mat labels);
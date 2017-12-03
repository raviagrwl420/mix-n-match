#define USE_OPENCV_3 
//#define USE_OPENCV_2

#define _CRT_SECURE_NO_WARNINGS

#ifdef USE_OPENCV_3
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect.hpp"
#include <opencv2/ml.hpp>
#include <string>
#endif

#ifdef USE_OPENCV_2
#include <cv.h>
#include <highgui.h>
#include <opencv2/ml/ml.hpp>
#endif

#include <iostream>
#include <dirent.h>


#ifdef USE_OPENCV_3
using namespace cv::ml;
#endif
using namespace cv;
using namespace std;

vector<pair<Mat, int>> trainData1;
vector<pair<Mat, int>> trainData2;
vector<pair<Mat, int>> trainData3;

vector<pair<Mat, int>> testData1;
vector<pair<Mat, int>> testData2;
vector<pair<Mat, int>> testData3;

// HOGDescriptor hog(
//         Size(20,20), //winSize
//         Size(10,10), //blocksize
//         Size(5,5), //blockStride,
//         Size(10,10), //cellSize,
//                  9, //nbins,
//                   1, //derivAper,
//                  -1, //winSigma,
//                   0, //histogramNormType,
//                 0.2, //L2HysThresh,
//                   0,//gammal correction,
//                   64,//nlevels=64
//                   1);

// void getTrainTestHOG(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, vector<pair<Mat,int>> &trainData, vector<pair<Mat,int>> &testData){

//     for(int i=0;i<trainData.size();i++){
//         vector<float> descriptors;
//     	hog.compute(trainData[y].first,descriptors);
//     	trainHOG.push_back(descriptors);
//     }

//     for(int j=0;j<testData.size();j++){

//         vector<float> descriptors;
//     	hog.compute(testData[j],descriptors);
//     	testHOG.push_back(descriptors);
//     } 
// }

void getTrainTest(vector<pair<Mat, int>> pView, vector<pair<Mat, int>> nView, vector<pair<Mat, int>> &trainData, vector<pair<Mat, int>> &testData) {

	int ptrain_size = 0.9*pView.size();
	int ptest_size = 0.1*pView.size();

	int ntrain_size = 0.9*nView.size();
	int ntest_size = 0.1*nView.size();

	printf("I AM HERE");

	// vector<pair<Mat,int> trainData, testData;

	vector<pair<Mat, int>> ptrain(pView.begin(), pView.begin() + ptrain_size);
	vector<pair<Mat, int>>ptest(pView.begin() + ptrain_size, pView.end());

	vector<pair<Mat, int>> ntrain(nView.begin(), nView.begin() + ntrain_size);
	vector<pair<Mat, int>> ntest(nView.begin() + ntrain_size, nView.end());

	trainData.insert(trainData.end(), ptrain.begin(), ptrain.end());
	trainData.insert(trainData.end(), ntrain.begin(), ntrain.end());

	testData.insert(testData.end(), ptest.begin(), ptest.end());
	testData.insert(testData.end(), ntest.begin(), ntest.end());

	std::random_shuffle(trainData.begin(), trainData.end());

	std::random_shuffle(testData.begin(), testData.end());

}



void loadData(string &pathPositive, string &pathNegative) {


	DIR *dir;

	vector<pair<Mat, int>> pView1;
	vector<pair<Mat, int>> pView2;
	vector<pair<Mat, int>> pView3;

	vector<pair<Mat, int>> nView1;
	vector<pair<Mat, int>> nView2;
	vector<pair<Mat, int>> nView3;

	pair<Mat, int> check;


	// int err = 0;
	//    zip *z = zip_open("../training_data/training.zip", 0, &err);

	//    const char *name = "positive/";
	//    struct zip_stat st;
	//    zip_stat_init(&st);
	//    zip_stat(z, name, 0, &st);

	

	//Mat img = imread(pathName, CV_LOAD_IMAGE_GRAYSCALE);

	struct dirent *ent;
	const char* path = pathPositive.c_str();
	//string path = pathPositive;
	if ((dir = opendir(path)) != NULL) {

		while ((ent = readdir(dir)) != NULL) {

			printf("%s\n", ent->d_name);
			char* pathName = new char;

			strcpy(pathName, path);
			strcat(pathName, "/");
			strcat(pathName, ent->d_name);



			Mat img = imread(pathName, CV_LOAD_IMAGE_GRAYSCALE);
			int ImgCount = 0;

			if (atoi(ent->d_name) % 3 == 1) {

				pView1.push_back(make_pair(img, 1));

			}
			else if (atoi(ent->d_name) % 3 == 2) {

				pView2.push_back(make_pair(img, 1));
			}
			else {

				pView3.push_back(make_pair(img, 1));
			}

		}

	}
	else { printf("check your folder: Postive"); }

	check = pView3[1];



	const char* path2 = pathNegative.c_str();
	if ((dir = opendir(path2)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			// printf ("%s\n", ent->d_name);
			char* pathName2 = new char;

			strcpy(pathName2, path2);
			strcat(pathName2, "/");
			strcat(pathName2, ent->d_name);


			Mat img = imread(pathName2, CV_LOAD_IMAGE_GRAYSCALE);
			int ImgCount = 0;

			if (atoi(ent->d_name) % 3 == 1) {

				nView1.push_back(make_pair(img, 0));

			}
			else if (atoi(ent->d_name) % 3 == 2) {

				nView2.push_back(make_pair(img, 0));
			}
			else {

				nView3.push_back(make_pair(img, 0));
			}

		}

	}
	else {

		printf("check your folder: Negative");

	}

	getTrainTest(pView1, nView1, trainData1, testData1);
	getTrainTest(pView2, nView2, trainData2, testData2);
	getTrainTest(pView3, nView3, trainData3, testData3);


}

// void ConvertVectortoMatrix(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, Mat &trainMat, Mat &testMat)
// {

//     int descriptor_size = trainHOG[0].size();

//     for(int i = 0;i<trainHOG.size();i++){
//         for(int j = 0;j<descriptor_size;j++){
//            trainMat.at<float>(i,j) = trainHOG[i][j]; 
//         }
//     }
//     for(int i = 0;i<testHOG.size();i++){
//         for(int j = 0;j<descriptor_size;j++){
//             testMat.at<float>(i,j) = testHOG[i][j]; 
//         }
//     }
// }

// void getSVMParams(SVM *svm)
// {
//     cout << "Kernel type     : " << svm->getKernelType() << endl;
//     cout << "Type            : " << svm->getType() << endl;
//     cout << "C               : " << svm->getC() << endl;
//     cout << "Degree          : " << svm->getDegree() << endl;
//     cout << "Nu              : " << svm->getNu() << endl;
//     cout << "Gamma           : " << svm->getGamma() << endl;
// }

// void SVMtrain(Mat &trainMat,vector<int> &trainLabels, Mat &testResponse,Mat &testMat){
// #ifdef USE_OPENCV_2
//     CvSVMParams params;
//     params.svm_type    = CvSVM::C_SVC;
//     params.kernel_type = CvSVM::RBF;
//     params.gamma = 0.50625;
//     params.C = 2.5;
//     CvSVM svm;
//     CvMat tryMat = trainMat;
//     Mat trainLabelsMat(trainLabels.size(),1,CV_32FC1);

//     for(int i = 0; i< trainLabels.size();i++){
//         trainLabelsMat.at<float>(i,0) = trainLabels[i];
//     }
//     CvMat tryMat_2 = trainLabelsMat;
//     svm.train(&tryMat,&tryMat_2, Mat(), Mat(), params);
//     svm.predict(testMat,testResponse);
// #endif
// #ifdef USE_OPENCV_3
//     Ptr<SVM> svm = SVM::create();
//     svm->setGamma(0.50625);
//     svm->setC(12.5);
//     svm->setKernel(SVM::RBF);
//     svm->setType(SVM::C_SVC);
//     Ptr<TrainData> td = TrainData::create(trainMat, ROW_SAMPLE, trainLabels);
//     svm->train(td);
//     //svm->trainAuto(td);
//     svm->save("model4.yml");
//     svm->predict(testMat, testResponse);
//     getSVMParams(svm);
// #endif
// }
// void SVMevaluate(Mat &testResponse,float &count, float &accuracy,vector<int> &testLabels){

//     for(int i=0;i<testResponse.rows;i++)
//     {
//         //cout << testResponse.at<float>(i,0) << " " << testLabels[i] << endl;
//         if(testResponse.at<float>(i,0) == testLabels[i]){
//             count = count + 1;
//         }  
//     }
//     accuracy = (count/testResponse.rows)*100;
// }

int main() {


	std::string pathPositive = "./training_data/training/positive";
	std::string pathNegative = "./training_data/training/negative";

	loadData(pathPositive, pathNegative);

	//work with one model
	// you have got trainData1 and testData1

	// std::vector<Mat > trainData;
	// std::vector<Mat> testData;
	// std::vector<int > trainLabels;
	// std::vector<int> testLabels;
	// for (int i =0; i< trainData1.size(); i++){

	// 	trainData.push_back(trainData1[i].first);
	// 	trainLabels.push_back(trainData1[i].second)

	// }

	//  for (int i =0; i< testData1.size(); i++){

	// 	testData.push_back(testData1[i].first);
	// 	testLabels.push_back(testData1[i].second)

	// }
	// std::vector<std::vector<float> > trainHOG;
	// std::vector<std::vector<float> > testHOG;

	// getTrainTestHOG(trainHOG,testHOG,trainData,testData);

	// int descriptor_size = trainHOG[0].size();
	// cout << "Descriptor Size : " << descriptor_size << endl;

	// Mat trainMat(trainHOG.size(),descriptor_size,CV_32FC1);
	// Mat testMat(testHOG.size(),descriptor_size,CV_32FC1);

	// ConvertVectortoMatrix(trainHOG,testHOG,trainMat,testMat);

	// Mat testResponse;
	// SVMtrain(trainMat,trainLabels,testResponse,testMat); 


	// float count = 0;
	// float accuracy = 0 ;
	// SVMevaluate(testResponse,count,accuracy,testLabels);

	// cout << "Accuracy        : " << accuracy << "%"<< endl;
	return 0;
}



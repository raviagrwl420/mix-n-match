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

int modelCount = 1;

HOGDescriptor hog(
	Size(224, 224), //winSize
	Size(112, 112), //blocksize
	Size(28, 28), //blockStride,
	Size(56, 56), //cellSize,
	9, //nbins,
	1, //derivAper,
	-1, //winSigma,
	0, //histogramNormType,
	0.2, //L2HysThresh,
	0, //gammal correction,
	64, //nlevels=64
	1
);

void getTrainTestHOG(vector<vector<float>> &trainHOG, vector<vector<float>> &testHOG, 
	vector<Mat> &trainData, vector<Mat> &testData) {

    for(int i=0; i<trainData.size(); i++) {
        vector<float> descriptors;
    	hog.compute(trainData[i], descriptors);
    	trainHOG.push_back(descriptors);
    }

    for(int j=0; j<testData.size(); j++){
        vector<float> descriptors;
    	hog.compute(testData[j], descriptors);
    	testHOG.push_back(descriptors);
    }
}

void getTrainTest(vector<pair<Mat, int>> pView, vector<pair<Mat, int>> nView, vector<pair<Mat, int>> &trainData, vector<pair<Mat, int>> &testData) {
	int ptrain_size = 0.9*pView.size();
	int ptest_size = 0.1*pView.size();

	int ntrain_size = 0.9*nView.size();
	int ntest_size = 0.1*nView.size();

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

float computeResponse(Mat view,int model){

	vector<float> descriptors;
	hog.compute(view, descriptors);
	Mat descMat(1, descriptors.size(), CV_32FC1);
	for(int j = 0; j< descriptors.size(); j++){
        descMat.at<float>(0,j) = descriptors[j]; 
    }

    float response;
    Ptr<SVM> svm;

    if (model == 1) {
		svm = Algorithm::load<SVM>("model1.yml");
	} else if (model == 2) {
		svm = Algorithm::load<SVM>("model2.yml");
	} else if (model == 3) {
		svm = Algorithm::load<SVM>("model3.yml");
	}

	response = svm->predict(descMat,cv::noArray(),cv::ml::StatModel::RAW_OUTPUT);
	// testResponse = svm->predict(descMat);

	return response;


}


float checkPlausible(Mat view1, Mat view2, Mat view3){

	float response1, response2, response3, finalResponse;

	response1 = computeResponse(view1,1);
	response2 = computeResponse(view2,2);
	response3 = computeResponse(view3,3);

	if(response1 == 1 && response2 == 1 && response3 == 1 ){
		finalResponse = 1;
	}else{
		finalResponse = 0;
	}

	return finalResponse;
}

void loadData(vector<pair<Mat, int>> &view1, vector<pair<Mat, int>> &view2, vector<pair<Mat, int>> &view3, string &pathName, int label){

	DIR *dir;

	struct dirent *ent;
	const char* path = pathName.c_str();

	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (string(ent->d_name) == string(".") || string(ent->d_name) == string(".."))
				continue;

			string pathName = path + string("/") + ent->d_name;
			Mat img = imread(pathName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
			
			if (atoi(ent->d_name) % 3 == 1) {
				view1.push_back(make_pair(img, label));
			} else if (atoi(ent->d_name) % 3 == 2) {
				view2.push_back(make_pair(img, label));
			} else if (atoi(ent->d_name) % 3 == 0) {
				view3.push_back(make_pair(img, label));
			}
		}
	} else { 
		printf("check your folder\n"); 
	}
}



void loadPositiveNegativeData(string &pathPositive, string &pathNegative) {
	vector<pair<Mat, int>> pView1;
	vector<pair<Mat, int>> pView2;
	vector<pair<Mat, int>> pView3;

	vector<pair<Mat, int>> nView1;
	vector<pair<Mat, int>> nView2;
	vector<pair<Mat, int>> nView3;


	loadData(pView1, pView2, pView3, pathPositive,1);
	loadData(nView1, nView2, nView3, pathNegative,0);

	// vector<pair<Mat, int>> view1;
	// vector<pair<Mat, int>> view2;
	// vector<pair<Mat, int>> view3;


	getTrainTest(pView1, nView1, trainData1, testData1);
	getTrainTest(pView2, nView2, trainData2, testData2);
	getTrainTest(pView3, nView3, trainData3, testData3);
}

void ConvertVectortoMatrix(vector<vector<float> > &trainHOG, vector<vector<float> > &testHOG, Mat &trainMat, Mat &testMat) {
    int descriptor_size = trainHOG[0].size();

    for(int i = 0; i<trainHOG.size(); i++) {
        for(int j = 0; j<descriptor_size; j++){
           trainMat.at<float>(i,j) = trainHOG[i][j]; 
        }
    }

    for(int i = 0; i<testHOG.size(); i++) {
        for(int j = 0; j<descriptor_size; j++){
            testMat.at<float>(i,j) = testHOG[i][j]; 
        }
    }
}

void getSVMParams(SVM *svm)
{
    cout << "Kernel type     : " << svm->getKernelType() << endl;
    cout << "Type            : " << svm->getType() << endl;
    cout << "C               : " << svm->getC() << endl;
    cout << "Degree          : " << svm->getDegree() << endl;
    cout << "Nu              : " << svm->getNu() << endl;
    cout << "Gamma           : " << svm->getGamma() << endl;
}

void SVMtrain(Mat &trainMat,vector<int> &trainLabels, Mat &testResponse,Mat &testMat, String modelName){
#ifdef USE_OPENCV_2
    CvSVMParams params;
    params.svm_type    = CvSVM::C_SVC;
    params.kernel_type = CvSVM::RBF;
    params.gamma = 0.50625;
    params.C = 2.5;
    CvSVM svm;
    CvMat tryMat = trainMat;
    Mat trainLabelsMat(trainLabels.size(),1,CV_32FC1);

    for(int i = 0; i< trainLabels.size();i++){
        trainLabelsMat.at<float>(i,0) = trainLabels[i];
    }
    CvMat tryMat_2 = trainLabelsMat;
    svm.train(&tryMat,&tryMat_2, Mat(), Mat(), params);
    svm.predict(testMat,testResponse);
#endif
#ifdef USE_OPENCV_3
    Ptr<SVM> svm = SVM::create();
    svm->setGamma(0.50625);
    svm->setC(12.5);
    svm->setKernel(SVM::RBF);
    svm->setType(SVM::C_SVC);
    Ptr<TrainData> td = TrainData::create(trainMat, ROW_SAMPLE, trainLabels);
    svm->train(td);
    //svm->trainAuto(td);
    svm->save(modelName);
    svm->predict(testMat, testResponse);
    getSVMParams(svm);
#endif
}





void SVMevaluate(Mat &testResponse,float &count, float &accuracy,vector<int> &testLabels){

    for(int i=0;i<testResponse.rows;i++)
    {
        if(testResponse.at<float>(i,0) == testLabels[i]){
            count = count + 1;
        }  
    }
    accuracy = (count/testResponse.rows)*100;
}


void trainModel(vector<pair<Mat, int>> trainPair, vector<pair<Mat, int>> testPair, String modelName)
{
	std::vector<Mat> trainData;
	std::vector<Mat> testData;
	std::vector<int> trainLabels;
	std::vector<int> testLabels;

	for (int i =0; i< trainData1.size(); i++) {
		trainData.push_back(trainPair[i].first);
		trainLabels.push_back(trainPair[i].second);
	}

	for (int i =0; i< testData1.size(); i++) {
		testData.push_back(testPair[i].first);
		testLabels.push_back(testPair[i].second);
	}

	vector<vector<float>> trainHOG;
	vector<vector<float>> testHOG;

	getTrainTestHOG(trainHOG, testHOG, trainData, testData);

	int descriptor_size = trainHOG[0].size();

	Mat trainMat(trainHOG.size(), descriptor_size, CV_32FC1);
	Mat testMat(testHOG.size(), descriptor_size, CV_32FC1);

	// cout<<"TrainMat: "<<trainMat.size()<<endl;
	// cout<<"TestMat: "<<testMat.size()<<endl;

	ConvertVectortoMatrix(trainHOG, testHOG, trainMat, testMat);

	Mat testResponse;
	SVMtrain(trainMat, trainLabels, testResponse, testMat, modelName); 

	float count = 0;
	float accuracy = 0 ;
	SVMevaluate(testResponse, count, accuracy, testLabels);

	cout << "Accuracy of model "<<modelCount<<" : " << accuracy << "%"<< endl;

	modelCount++;


}
int main(int argc, char **argv) {

	if(argc ==1){
	std::string pathPositive = "../training_data/training/positive";
	std::string pathNegative = "../training_data/training/negative";

	loadPositiveNegativeData(pathPositive, pathNegative);

	//work with one model
	// you have got trainData1 and testData1

	trainModel(trainData1,testData1, "model1.yml");
	trainModel(trainData2,testData2, "model2.yml");
	trainModel(trainData3, testData3,"model3.yml");
   } else

   {

   	std::string path = argv[1];
   	Mat img = imread(path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
   	vector<float> descriptors;
    hog.compute(img, descriptors);

    Mat descMat(1, descriptors.size(), CV_32FC1);

        for(int j = 0; j< descriptors.size(); j++){
           descMat.at<float>(0,j) = descriptors[j]; 
        }
   
    float testResponse;
    Ptr<SVM> svm;

    if (atoi(path.c_str()) % 3 == 1) {
		svm = Algorithm::load<SVM>("model1.yml");
	} else if (atoi(path.c_str()) % 3 == 2) {
		svm = Algorithm::load<SVM>("model2.yml");
	} else if (atoi(path.c_str()) % 3 == 0) {
		svm = Algorithm::load<SVM>("model3.yml");
	}

	// testResponse = svm->predict(descMat,cv::noArray(),cv::ml::StatModel::RAW_OUTPUT);
	testResponse = svm->predict(descMat);

	cout << "testResponse: " << testResponse << endl;

   



   }

	
	return 0;

}



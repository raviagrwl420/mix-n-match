#include <kmean.h>



HOGDescriptor hogk(
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



void getLabels(vector<PartBase*> chairs){

	vector<Mat> view1, view2, view3;
	Mat labels1, labels2, labels3;

	for(int i = 0 ; i<chairs.size();i++){

		view1.push_back(chairToMat(chairs[i],SIDE));
		view2.push_back(chairToMat(chairs[i],TOP));
		view3.push_back(chairToMat(chairs[i],FRONT));

	}

	// vector<vector<float>> hogVectors1, hogVectors2, hogVectors3;

	// getHOG(hogVectors1, view1);
	// getHOG(hogVectors2, view2);
	// getHOG(hogVectors3, view3);



	labels1 = calculateKMeans(view1);
	labels2 = calculateKMeans(view2);
	labels3 = calculateKMeans(view3);

	cout<<"Labels1 size: \t"<< labels1.size();
	cout<<"Labels2 size: \t"<< labels2.size();
	cout<<"Labels3 size: \t"<< labels3.size();



}



Mat chairToMat(PartBase *part, View view){

	unsigned char *projection = OffScreenRenderer::getProjection(part, view);
	Mat projectionMatrix(WIDTH, HEIGHT, CV_8UC1);

	for (int i = HEIGHT - 1; i >= 0; i--) {
		for (int j = 0; j < WIDTH; j++) {
			int index = i*WIDTH + j;
			int x = HEIGHT - 1 - i;
			int y = j;
			projectionMatrix.at<uchar>(x, y) = projection[index];
		}
	}

	return projectionMatrix;
}


Mat calculateKMeans(vector<Mat> view){
	

	vector<vector<float>> hogSamples;
	getHOG(hogSamples, view);


	Mat samples;
	ConvertVectortoMatrix(hogSamples,samples);


	int clusterCount = 3;

	Mat labels;
  	int attempts = 5;
  	Mat centers;

  	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 100, 0.0001), attempts, KMEANS_PP_CENTERS, centers );

  	return labels;

}




void ConvertVectortoMatrix (vector<vector<float>> &hogSamples,  Mat &samples) {
	int descriptor_size = hogSamples[0].size();

	for (int i = 0; i < hogSamples.size(); i++) {
		for (int j = 0; j < descriptor_size; j++){
			samples.at<float>(i, j) = hogSamples[i][j]; 
		}
	}

}

void loadData (vector<Mat> &view1, vector<Mat> &view2, vector<Mat> &view3, string &pathName) {
	DIR *dir;

	struct dirent *ent;
	const char* path = pathName.c_str();

	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			if (string(ent->d_name) == string(".") || string(ent->d_name) == string("..")) {
				continue;
			}

			string pathName = path + string("/") + ent->d_name;
			Mat img = imread(pathName.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
			
			if (atoi(ent->d_name) % 3 == 1) {
				view1.push_back(img);
			} else if (atoi(ent->d_name) % 3 == 2) {
				view2.push_back(img);
			} else if (atoi(ent->d_name) % 3 == 0) {
				view3.push_back(img);
			}
		}
	} else { 
		printf("Check your folder\n"); 
	}
}



//function for k-mean clustering

void getHOG(vector<vector<float>> &hogVectors, vector<Mat> &samples){

	for (int i = 0; i < samples.size(); i++) {
		vector<float> descriptors;
		hogk.compute(samples[i], descriptors);
		hogVectors.push_back(descriptors);
	}


}
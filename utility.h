#ifndef UTILITY_H
#define UTILITY_H


#include <opencv2/opencv.hpp>
#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>
#include <deque>
//#include <dirent.h>
#include <io.h>
#include <math.h>
#include <algorithm>
#include <limits>

// tbb related libraries
//#include "tbb/tbb.h"
//#include "tbb/task_scheduler_init.h"
//#include "tbb/tick_count.h"

//using namespace std;
//using namespace cv;

//const double PI = 3.14159265;
//const double PI = 3.1415926536;
const double PI = 3.1415926535897932384626433832795;
// canny edge operation on input image
void canny(cv::Mat&, cv::Mat&);
// make the border of the the image as a special color
void makeSpecialColorBorder(cv::Mat& output, int borderWidth, cv::Scalar color);
// assign color/255 to a specific location on color/gray image
void assignValueOnImage(cv::Mat& image, int row, int col, cv::Scalar color);
// only draw one Trajectory
void drawTrajectory(std::vector<cv::Point2f> points, cv::Mat& img, cv::Scalar color);
// read the directories files names
void readDirectory( const std::string& directoryName, std::vector<std::string>& filenames, bool addDirectoryName=true );
// calculate foreground percent according to a bkgd binary images
double calForePercent(const cv::Mat& output);
//compare covariance matricies
float compareCov(cv::Mat a, cv::Mat b);
//calculate hog features
//based on code from http://www.mathworks.com/matlabcentral/fileexchange/33863
cv::Mat HOGfeatures(cv::Mat & img, int nb_bins = 9, double cheight = 8, double cwidth = 8, 
	int block_rows = 2, int block_cols = 2, bool orient = 0, double clip_val = 0.2);

#endif // UTILITY_H

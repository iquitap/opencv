#include <opencv2\opencv.hpp>
#include <highgui.h>
#include <iostream>

using namespace std;
using namespace cv;

Mat g_srcImage, g_srcGrayImage, g_dstImage1, g_dstImage2, g_dstImage3;

//Canny
Mat g_cannyDetectedEdges;
int g_cannyLowThreshold = 1;

//Sobel
Mat g_sobelGradient_X, g_sobelGradient_Y;
Mat g_sobelAbsGradient_X, g_sobelAbsGradient_Y;
int g_sobelKernelSize = 1;

//Scharr
Mat g_scharrGradient_X, g_scharrGradient_Y;
Mat g_scharrAbsGradient_X, g_scharrAbsGradient_Y;

static void on_Canny(int, void*);
static void on_Sobel(int, void*);
void Scharr();
int main()
{
	system("color 2F");

	g_srcImage = imread("D:\\testtest.jpg", 1);
	if (!g_srcImage.data) { cout << "error\n" << endl; return false; }

	namedWindow("原图", 1);
	imshow("原图", g_srcImage);

	g_dstImage1.create(g_srcImage.size(), g_srcImage.type());
	g_dstImage2.create(g_srcImage.size(), g_srcImage.type());
	g_dstImage3.create(g_srcImage.size(), g_srcImage.type());

	cvtColor(g_srcImage, g_srcGrayImage, CV_BGR2GRAY);

	imshow("Gray", g_srcGrayImage);

	namedWindow("Canny", 1);
	namedWindow("Sobel", 1);

	createTrackbar("CannyLowThreshold: ", "Canny", &g_cannyLowThreshold, 120, on_Canny);
	createTrackbar("SobelKernelSize: ", "Sobel", &g_sobelKernelSize, 3, on_Sobel);

	on_Canny(0, 0);
	on_Sobel(0, 0);
	Scharr();

	cout << "q to exit " << endl;
	while (char(waitKey(1)) != 'q') {}

	return 0;
}

static void on_Canny(int, void*)
{
	//3*3内核均值滤波降噪
	blur(g_srcGrayImage, g_cannyDetectedEdges, Size(3, 3));

	Canny(g_cannyDetectedEdges, g_cannyDetectedEdges, g_cannyLowThreshold, g_cannyLowThreshold * 3, 3);
	g_dstImage1 = Scalar::all(0);
	g_srcImage.copyTo(g_dstImage1, g_cannyDetectedEdges);
	imshow("Canny", g_dstImage1);
}

static void on_Sobel(int, void*)
{
	Sobel(g_srcImage, g_sobelGradient_X, CV_16S, 1, 0, (2 * g_sobelKernelSize + 1), 1, 1, BORDER_DEFAULT);
	Sobel(g_srcImage, g_sobelGradient_Y, CV_16S, 0, 1, (2 * g_sobelKernelSize + 1), 1, 1, BORDER_DEFAULT);

	convertScaleAbs(g_sobelGradient_X, g_sobelAbsGradient_X);
	convertScaleAbs(g_sobelGradient_Y, g_sobelAbsGradient_Y);

	addWeighted(g_sobelAbsGradient_X, 0.5, g_sobelAbsGradient_Y, 0.5, 0, g_dstImage2);

	imshow("Sobel", g_dstImage2);
}

void Scharr()
{
	Scharr(g_srcImage, g_scharrGradient_X, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	Scharr(g_srcImage, g_scharrGradient_Y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);

	convertScaleAbs(g_scharrGradient_X, g_scharrAbsGradient_X);
	convertScaleAbs(g_scharrGradient_Y, g_scharrAbsGradient_Y);

	//addWeighted(g_scharrAbsGradient_X, 0.5, g_scharrAbsGradient_Y, 0.5, 0, g_dstImage3);
	imshow("Scharr", g_dstImage3);
}

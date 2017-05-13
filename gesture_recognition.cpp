#include <windows.h>
#include <opencv2\opencv.hpp>
#include <highgui.h>
#include <iostream>
#include <cv.h>
#include <cxcore.h>

#define WIN_SRC "Src"
#define WIN_RESULT "Dst q to exit"
using namespace std;
using namespace cv;
int main()
{
	VideoCapture cap(0);

	bool bHandFlag = false;

	namedWindow(WIN_SRC, CV_WINDOW_AUTOSIZE);
	namedWindow(WIN_RESULT, CV_WINDOW_AUTOSIZE);

	Mat frame;	
	Mat frameHSV;	
	Mat mask(frame.rows, frame.cols, CV_8UC1);	
	Mat dst(frame);

	vector< vector<Point> > contours;
	vector< vector<Point> > filterContours;	
	vector< Vec4i > hierarchy;	
	vector< Point > hull;	


	int count = 0;
	int previousX = 0;
	int previousY = 0;
	int presentX = 0;
	int presentY = 0;
	bool lastImgHasHand = false;
	while (true)
	{
		int minX = 320;
		int maxX = 240;
		int minY = 320;
		int maxY = 240;

		cap >> frame;
		if (frame.empty())
		{
			cout << "error";
			break;
		}
		imshow(WIN_SRC, frame);

		medianBlur(frame, frame, 5);
		cvtColor(frame, frameHSV, CV_BGR2HSV);

		Mat dstTemp1(frame.rows, frame.cols, CV_8UC1);
		Mat dstTemp2(frame.rows, frame.cols, CV_8UC1);
		inRange(frameHSV, Scalar(0, 30, 30), Scalar(40, 170, 256), dstTemp1);
		inRange(frameHSV, Scalar(156, 30, 30), Scalar(180, 170, 256), dstTemp2);
		bitwise_or(dstTemp1, dstTemp2, mask);

		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		erode(mask, mask, element);
		morphologyEx(mask, mask, MORPH_OPEN, element);
		dilate(mask, mask, element);
		morphologyEx(mask, mask, MORPH_CLOSE, element);
		frame.copyTo(dst, mask);
		contours.clear();
		hierarchy.clear();
		filterContours.clear();
		findContours(mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		for (size_t i = 0; i < contours.size(); i++)
		{
			if (fabs(contourArea(Mat(contours[i]))) > 30000)
			{
				filterContours.push_back(contours[i]);
			}
		}

		if (filterContours.size()>0)
		{

			count++;
			lastImgHasHand = true;
			drawContours(dst, filterContours, -1, Scalar(255, 0, 255), 3);

			for (size_t j = 0; j<filterContours.size(); j++)
			{
				convexHull(Mat(filterContours[j]), hull, true);
				int hullcount = (int)hull.size();
				for (int i = 0; i<hullcount - 1; i++)
				{
					line(dst, hull[i + 1], hull[i], Scalar(255, 255, 255), 2, CV_AA);
					//printf("num%d:x=%d\ty=%d\t\n", i, hull[i].x, hull[i].y);
					if (hull[i].x>maxX)
						maxX = hull[i].x;
					if (hull[i].x<minX)
						minX = hull[i].x;
					if (hull[i].y>maxY)
						maxY = hull[i].y;
					if (hull[i].y<minY)
						minY = hull[i].y;
					//printf("minX=%d\tminY=%d\tmaxX=%d\tmaxY=%d\t\n", minX, minY, maxX, maxY);

				}

				line(dst, hull[hullcount - 1], hull[0], Scalar(0, 0, 255), 2, CV_AA);

				if (count == 1)
				{
					previousX = (minX + maxX) / 2;
					//printf("previousX=%d\n", previousX);
					previousY = (minY + maxY) / 2;
					//printf("previousY=%d\n", previousY);
				}
				else
				{
					presentX = (minX + maxY) / 2;
					presentY = (minY + maxY) / 2;
				}
			}
		}
		else
		{
			if (lastImgHasHand == true)
			{
				if ((previousX - presentX)<0)
				{
					printf("\t\tleft");
					keybd_event(VK_LEFT, 0, 0, 0);
					keybd_event(VK_LEFT, 0, KEYEVENTF_KEYUP, 0);
				}
				if ((previousX - presentX)>0)
				{
					printf("\t\tright");
					keybd_event(VK_RIGHT, 0, 0, 0);
					keybd_event(VK_RIGHT, 0, KEYEVENTF_KEYUP, 0);
				}
				if ((previousY - presentY)<0)
				{
					printf("\t\tdown\n\n");
				}
				if ((previousY - presentY)>0)
				{
					printf("\t\tup\n\n");
				}
				printf("\n");
				count = 0;
				lastImgHasHand = false;
			}
		}

		imshow(WIN_RESULT, dst);
		dst.release();


		if (char(waitKey(1)) == 'q')
			break;
	}
}

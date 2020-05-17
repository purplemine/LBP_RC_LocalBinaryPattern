#include <iostream>
using std::endl;
using std::cout;
#include "LBP/LBPBase.hpp"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include "omp.h"


#define DBL_TM true
double fn_basic_lbp(const cv::Mat& src, cv::Mat& dst);
double fn_circular_lbp(const cv::Mat& src, cv::Mat& dst);
double fn_multiblock_lbp(const cv::Mat& src, cv::Mat& dst);

typedef double (*Func)(const cv::Mat& src, cv::Mat& dst);

int main()
{
	std::string name = "./test.jpg";
	cv::Mat img_org = cv::imread(name);
	cv::Mat img;
	double rate = 0.8;
	cv::resize(img_org, img, cv::Size(img_org.cols * rate, img_org.rows * rate));
	
	
	std::vector<Func> funcs;
	funcs.push_back(&fn_basic_lbp);
	funcs.push_back(&fn_circular_lbp);
	funcs.push_back(&fn_multiblock_lbp);
	
	// #pragma omp parallel for
	for (int i = 0; i < 3; ++i)
	{
		cv::Mat dst;
		double sum_basic_lbp = funcs[i](img, dst);
		if (DBL_TM)
		{
			cout << "avg_basic_lbp costs " << sum_basic_lbp << ".s " << endl;
		}
	}
	cv::waitKey(0);
	return 0;
}

double fn_basic_lbp(const cv::Mat& src, cv::Mat& dst)
{
	LBP::LBP lbpopt(LBP::BASIC_LBP);
	std::vector<cv::Mat> dsts;
	std::vector<cv::Mat> dsts2;
	cv::split(src, dsts);
	double sum = 0.0;
	int cnt = 0;
	for (size_t i = 0; i < dsts.size(); ++i)
	{
		dsts2.push_back(cv::Mat());
		double src_tm = omp_get_wtime();
		bool ret = lbpopt.run_mutable(dsts[i], dsts2[dsts2.size() - 1]);
		double cost = omp_get_wtime() - src_tm;
		sum += cost;
		cnt += 1;
		if (DBL_TM)
		{
			cout << "fn_basic_lbp " << i << " costs " << cost << ".s " << endl;
		}
		cv::imshow("basic_lbp", dsts2[dsts2.size() - 1]);
		cv::waitKey(120);
	}
	if (dsts2.empty())
		return sum;
	if (dsts2.size() == 3)
	{
		int rows = dsts2[0].rows;
		int cols = dsts2[0].cols;
		dst = cv::Mat::zeros(rows, cols, CV_8UC3);
		for (int i = 0; i < rows; ++i)
		{
			for (int j = 0; j < cols; ++j)
			{
				dst.at<cv::Vec3b>(i, j)[0] = dsts2[0].at<unsigned char>(i, j);
				dst.at<cv::Vec3b>(i, j)[1] = dsts2[1].at<unsigned char>(i, j);
				dst.at<cv::Vec3b>(i, j)[2] = dsts2[2].at<unsigned char>(i, j);
			}
		}
	}
	cv::imshow("basic_lbp", dst);
	cv::waitKey(500);
	return sum/cnt;
}
double fn_circular_lbp(const cv::Mat& src, cv::Mat& dst)
{
	int radius = 2;
	int angle = 0; 
	int neighbor = 7;
	double sum = 0.0;
	int cnt_cum = 0;
	cv::Mat dsts;
	cv::cvtColor(src, dsts, cv::COLOR_BGR2GRAY);

	while (neighbor < 9)
	{
		radius = 2;
		for (; radius < 5; ++radius)
		{
			angle = 0;
			for (; angle < neighbor; ++angle)
			{
				LBP::LBP lbpopt(LBP::CIRCULAR_LBP, radius, angle, neighbor);			
				cv::Mat dsts2;
				double src_tm = omp_get_wtime();
				bool ret = lbpopt.run_mutable(dsts, dsts2);
				double cost = omp_get_wtime() - src_tm;
				sum += cost;
				cnt_cum += 1;
				if (DBL_TM)
				{
					cout << "fn_circule_lbp costs " << cost << " .s " << endl;
				}
				cv::imshow("fn_circule_lbp", dsts2);
				cv::waitKey(300);
			}
		}
		neighbor++;
	}

	return sum/cnt_cum;
}
double fn_multiblock_lbp(const cv::Mat& src, cv::Mat& dst)
{
	int radius = 3;
	int angle = 0;

	double sum = 0.0;
	int cnt_cum = 0;
	cv::Mat dsts;
	cv::cvtColor(src, dsts, cv::COLOR_BGR2GRAY);

	for (; radius < 11; radius += 2)
	{
		angle = 0;
		for (; angle < 8; ++angle)
		{
			LBP::LBP lbpopt(LBP::MULTISCALE_BLOCK_LBP, radius, angle);
			cv::Mat dsts2;
			double src_tm = omp_get_wtime();
			bool ret = lbpopt.run_mutable(dsts, dsts2);
			double cost = omp_get_wtime() - src_tm;
			sum += cost;
			cnt_cum += 1;
			if (DBL_TM)
			{
				cout << "fn_multi_block_lbp costs " << cost << " .s " << endl;
			}
			cv::imshow("fn_multi_block_lbp", dsts2);
			cv::waitKey(220);
		}
	}

	return sum / cnt_cum;
}
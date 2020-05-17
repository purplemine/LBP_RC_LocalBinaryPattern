#include "LBPBase.hpp"

namespace LBP
{

namespace detail
{


BasicLBP::BasicLBP()
{

}
BasicLBP::~BasicLBP()
{

}
bool BasicLBP::run_mutable(const cv::Mat& src, cv::Mat& dst)
{
	int radius = 1;
	int rows = src.rows;
	int cols = src.cols;
	if (rows < 2 * radius || cols < 2 * radius || src.type() != 0) // 只支持灰度图
		return false;

	dst = cv::Mat(rows - 2 * radius, cols - 2 * radius, CV_8UC1);
	int x(0), y(0);
	int neighbor = 8;
	typedef unsigned char T;
	T center(0), edges(0);
	int pos_x[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int pos_y[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	for (int k = 0; k < neighbor; ++k)
	{
		x = pos_x[k];
		y = pos_y[k];
		for (int i = radius; i < src.rows - radius; ++i)
		{
			for (int j = radius; j < src.cols - radius; ++j)
			{
				center = src.at<T>(i, j);
				edges = src.at<T>(i + x, j + y);
				T signal = (edges > center) ? 1 : 0;
				dst.at<T>(i - radius, j - radius) |= signal << (neighbor - k - 1);
			}
		}
	}
	return true;
}




}
}
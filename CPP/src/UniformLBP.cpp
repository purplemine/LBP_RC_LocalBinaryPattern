#include "LBPBase.hpp"

namespace LBP
{

namespace detail
{

UniformLBP::UniformLBP(unsigned int radius, unsigned int neighbor) : _radius(int(radius)), _neigbor((int)neighbor)
{
	if (_radius <= 0)
		_radius = 1;
	if (_neigbor <= 0)
		_neigbor = 8;
}
UniformLBP::~UniformLBP()
{

}
bool UniformLBP::run_mutable(const cv::Mat& src, cv::Mat& dst)
{
	int rows = src.rows;
	int cols = src.cols;
	if (rows < 2 * _radius || cols < 2 * _radius || src.type() != 0 ) // 只支持灰度图
		return false;

	dst = cv::Mat::zeros(rows - 2 * _radius, cols - 2 * _radius, CV_8UC1);
	cv::Mat stat_dst = cv::Mat::zeros(rows - 2 * _radius, cols - 2 * _radius, CV_8UC1); // 直接用stat矩阵来存储状态
	
	float rx(0.0f), ry(0.0f), tx(0.0f), ty(0.0f);
	int x1(0), x2(0), y1(0), y2(0);
	float w1(0.0f), w2(0.0f), w3(0.0f), w4(0.0f);

	typedef unsigned char T;
	const float piece_angle = 360.0f / _neigbor; // Need to change loop repeat 
	T last_stat = 0;
	T pix_scale = _neigbor * (_neigbor - 1) + 2;
	T delta_scale = static_cast<T> (255/ pix_scale);
	for (int k = 0; k < _neigbor; ++k)
	{
		float current_angle = piece_angle * k;
		rx = static_cast<float>(std::cos(current_angle) * _radius);
		ry = static_cast<float>(std::sin(current_angle) * _radius);

		// 计算临近4个点
		x1 = static_cast<int>(std::floor(rx)); // 下取整
		x2 = static_cast<int>(std::ceil(rx));  // 上取整
		y1 = static_cast<int>(std::floor(ry));
		y2 = static_cast<int>(std::ceil(ry));

		// 近邻权重
		tx = rx - x1;
		ty = ry - y1;
		w1 = (1 - tx) * (1 - ty);
		w2 = tx  * (1 - ty);
		w3 = (1 - tx) *    ty;
		w4 = tx * ty;

		for (int i = _radius; i < src.rows - _radius; ++i)
		{
			for (int j = _radius; j < src.cols - _radius; ++j)
			{
				T center = src.at<T>(i, j);

				float middle = src.at<T>(i + x1, j + y1) * w1 \
					+ src.at<T>(i + x1, j + y2) * w2 \
					+ src.at<T>(i + x2, j + y1) * w3 \
					+ src.at<T>(i + x2, j + y2) * w4;
				T signal = (middle > center) ? 1 : 0;
				if (k == 0) 
				{
					stat_dst.at<T>(i - _radius, j - _radius) = signal;
					dst.at<T>(i - _radius, j - _radius) = delta_scale;
				}
				else
				{
					last_stat = stat_dst.at<T>(i - _radius, j - _radius);
					if (signal != last_stat)
					{
						stat_dst.at<T>(i - _radius, j - _radius) = signal;
						dst.at<T>(i - _radius, j - _radius) += delta_scale;
					}
				}
			}
		}
	}
	return true;
}

}
}



#include "LBPBase.hpp"

namespace LBP
{
namespace detail
{

CircularLBP::CircularLBP(unsigned int radius, unsigned int angle, unsigned int neighbor) :
	_radius((int)radius), _neigbor((int)neighbor), _angle((int)angle)
{
		
}
CircularLBP::~CircularLBP()
{

}

bool CircularLBP::run_mutable(const cv::Mat& src, cv::Mat& dst)
{
	int rows = src.rows;
	int cols = src.cols;
	if (rows < 2 * _radius || cols < 2 * _radius || src.type() != 0 || _neigbor < _angle ) // 只支持灰度图
		return false;

	dst = cv::Mat::zeros(rows - 2 * _radius, cols - 2 * _radius, CV_8UC1);
	
	float rx(0.0f), ry(0.0f), tx(0.0f), ty(0.0f);
	int x1(0), x2(0), y1(0), y2(0);
	float w1(0.0f), w2(0.0f), w3(0.0f), w4(0.0f);

	typedef unsigned char T;
	const float piece_angle = 360.0f / _neigbor;
	for (int k = 0; k < _neigbor; ++k)
	{
		unsigned idx = (k + _angle) % _neigbor;
		float current_angle = piece_angle * idx;
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
				dst.at<T>(i - _radius, j - _radius) |= signal << (_neigbor - k - 1);
			}
		}
	}
	return true;
}
}
}
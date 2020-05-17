#include "LBPBase.hpp"
#include <vector>
namespace LBP
{

namespace detail
{

MultiscaleBlockLBP::MultiscaleBlockLBP(unsigned int scale, unsigned int angle) : _scale(scale), _angle(angle)
{
	if ((_scale % 2) == 0)
		_scale += 1;
	_angle = _angle % 8;
}
MultiscaleBlockLBP::~MultiscaleBlockLBP()
{

}
bool MultiscaleBlockLBP::run_mutable(const cv::Mat& src, cv::Mat& dst)
{
	int rows = src.rows;
	int cols = src.cols;
	
	// 根据尺度计算网络信息
	int cellsz = _scale / 3;
	int cellar = cellsz * cellsz;
	int offset = cellsz / 2;

	int xCntRes = cols % cellsz;
	int xCntNum = cols / cellsz + (xCntRes == 0 ? 0 : 1);

	int yCntRes = rows % cellsz;
	int yCntNum = rows / cellsz + (yCntRes == 0 ? 0 : 1);

	if (xCntNum < 3 || yCntNum < 3 || src.type() != 0)
		return false;

	

	int xCurrentBlock(0), yCurrentBlock(0); // 当前block的左上顶点
	typedef unsigned char T;
	
	// get average pixel ahead
	std::vector<std::vector<double>> avg_pixel;
	avg_pixel.resize(xCntNum);
	std::vector<std::vector<T>> lbp_pixel;
	lbp_pixel.resize(xCntNum);
	for (int m = 0; m < xCntNum; ++m)
	{
		avg_pixel[m].resize(yCntNum, 0.0);
		lbp_pixel[m].resize(yCntNum, 0);
		for (int n = 0; n < yCntNum; ++n)
		{
			xCurrentBlock = cellsz * m;
			yCurrentBlock = cellsz * n;
			if (m == (xCntNum - 1) || n == (xCntNum - 1))
			{
				int totalpix = 0;
				int rowsL = yCntRes == 0 ? cellsz : yCntRes;
				int colsL = xCntRes == 0 ? cellsz : xCntRes;
				int resar = rowsL * colsL;
				for (int i = 0; i < rowsL; ++i)
				{
					for (int j = 0; j < xCntRes; ++j)
					{
						T val = src.at<T>(i + yCurrentBlock, j + xCurrentBlock);
						totalpix += (int)(val);
					}
				}
				avg_pixel[m][n] = (double)totalpix / (double)resar;
			}
			else
			{
				int totalpix = 0;
				for (int i = 0; i < cellsz; ++i)
				{
					for (int j = 0; j < cellsz; ++j)
					{
						T val = src.at<T>(i + yCurrentBlock, j + xCurrentBlock);
						totalpix += (int)(val);
					}
				}
				avg_pixel[m][n] = (double)totalpix / (double)cellar;
			}
			
		}
	}
	
	// cal true offset
	const int neighbor = 8;
	int angle = _angle;
	int direction_x[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
	int direction_y[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	int idx(0);
	int dx(0), dy(0);
	
	for (int k = 0; k < neighbor; ++k)
	{
		idx = (k + angle) % neighbor;
		dx = direction_x[idx];
		dy = direction_y[idx];
		
		for (int m = 1; m < xCntNum - 1; ++m)
		{
			for (int n = 1; n < yCntNum - 1; ++n)
			{
				T center = static_cast<T>(avg_pixel[m][n]);
				T middle = static_cast<T>(avg_pixel[m + dx][n + dy]);
				T signal = center < middle ? 1 : 0;
				lbp_pixel[m][n] |= signal << (neighbor - k - 1);
			}
		}
	}
	dst = cv::Mat(cellsz * (yCntNum - 2), cellsz * (xCntNum - 2), CV_8UC1);
	int cur_rows(0);
	int cur_cols(0);
	for (int m = 1; m < xCntNum - 1; ++m)
	{
		for (int n = 1; n < yCntNum - 1; ++n)
		{
			cur_rows = (n - 1) * cellsz;
			cur_cols = (m - 1) * cellsz;
			for (int cur_r = 0; cur_r < cellsz; ++cur_r)
			{
				for (int cur_c = 0; cur_c < cellsz; ++cur_c)
				{
					dst.at<T>(cur_r + cur_rows, cur_c + cur_cols) = lbp_pixel[m][n];
				}
			}
		}
	}

	return true;
}


}
}
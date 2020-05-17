#ifndef __LOCAL_BINARY_PATTERN_BASE_H__
#define __LOCAL_BINARY_PATTERN_BASE_H__

#include <opencv2/core.hpp>
#include <string>
namespace LBP
{
////////////////////////////////////////////////////////////////////////
namespace base
{
class ILBP
{
public:
	ILBP()
	{

	}
	virtual ~ILBP()
	{

	}
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst) = 0;
	virtual bool run_immutable(const cv::Mat& src, cv::Mat& dst)
	{
		return run_mutable(src, dst);
	}

	virtual std::string id()
	{
		return std::string("ILBP");
	}
};
}

////////////////////////////////////////////////////////////////////////
namespace detail // for implement
{
// Basic LBP
class BasicLBP : base::ILBP
{
public:
	BasicLBP();
	~BasicLBP();
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst);
private:

};
// Circular LBP
class CircularLBP : public base::ILBP
{
public:
	CircularLBP(unsigned int radius, unsigned int angle = 0, unsigned int neighbor = 8);
	~CircularLBP();
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst);
private:
	int _radius;
	int _neigbor;
	int _angle;
};

// Uniform LBP
class UniformLBP : public base::ILBP
{
public:
	UniformLBP(unsigned int radius, unsigned int neighbor = 8);
	~UniformLBP();
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst);

private:
	int _radius;
	int _neigbor;
};

// Multi-Scale Block LBP
class MultiscaleBlockLBP : public base::ILBP
{
public:
	MultiscaleBlockLBP(unsigned int scale, unsigned int angle = 0);
	~MultiscaleBlockLBP();
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst);
private:
	int _scale;
	int _angle;
};

}


////////////////////////////////////////////////////////////////////////
enum LBPAlg
{
	BASIC_LBP,
	CIRCULAR_LBP,
	MULTISCALE_BLOCK_LBP,
	UNIFORM_LBP
};

class LBP : public base::ILBP // for interface
{
public:
	LBP(LBPAlg type, unsigned int size = 3, unsigned int angle = 0, unsigned int neighbor = 8) : _impl(NULL)
	{
		if (type == BASIC_LBP)
		{
			_impl = (base::ILBP*)(new detail::BasicLBP());
		}
		else if (type == CIRCULAR_LBP)
		{
			_impl = (base::ILBP*)(new detail::CircularLBP(size, angle, neighbor));
		}
		else if (type == MULTISCALE_BLOCK_LBP)
		{
			_impl = (base::ILBP*)(new detail::MultiscaleBlockLBP(size, angle));
		}
		else if (type == UNIFORM_LBP)
		{
			_impl = (base::ILBP*)(new detail::UniformLBP(size, neighbor));
		}
		else
		{
			_impl = (base::ILBP*)(new detail::BasicLBP());
		}
	}

	virtual ~LBP()
	{
		if (_impl)
			delete _impl;
		_impl = NULL;
	}
	virtual bool run_mutable(const cv::Mat& src, cv::Mat& dst)
	{
		if (!_impl || src.type() != 0)
			return false;
		else 
			return _impl->run_mutable(src, dst);
	}
	virtual bool run_immutable(const cv::Mat& src, cv::Mat& dst)
	{
		if (!_impl)
			return false;
		else
			return _impl->run_immutable(src, dst);
	}
private:
	ILBP* _impl;
};

} // end namespace LBP



#endif
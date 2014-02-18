#define _USE_MATH_DEFINES
#include <cmath>

#include "fftw3.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

struct fft_test_fftw
{
	size_t size;
	fftw_plan plan;
	double *input;
	double *output;

	fft_test_fftw();
	explicit fft_test_fftw(size_t size);
	~fft_test_fftw();
	void create(size_t size);
	void destroy();
	void create_sine(double omega);
	void transform();
	double distance(const fft_test_fftw & other) const;
};

fft_test_fftw::fft_test_fftw():
	size(0),
	input(0),
	output(0),
	plan(0)
{
}

fft_test_fftw::fft_test_fftw(size_t size):
	size(0),
	input(0),
	output(0),
	plan(0)
{
	create(size);
}

fft_test_fftw::~fft_test_fftw()
{
	destroy();
}

void fft_test_fftw::create(size_t size)
{
	destroy();

	this->size = size;
	input = fftw_alloc_real(size);
	output = fftw_alloc_real(size);
	plan = fftw_plan_r2r_1d(size, input, output, FFTW_R2HC, FFTW_MEASURE | FFTW_PRESERVE_INPUT);
}

void fft_test_fftw::destroy()
{
	size = 0;

	if (plan != 0)
	{
		fftw_destroy_plan(plan);
		plan = 0;
	}

	if (input != 0)
	{
		fftw_free(input);
		input = 0;
	}

	if (output != 0)
	{
		fftw_free(output);
		output = 0;
	}
}

double fft_test_fftw::distance(const fft_test_fftw & other) const
{
	if (size != other.size) return std::numeric_limits<double>::quiet_NaN();

	double d = 0.0;

	for (size_t index = 0; index < size; ++index)
	{
		d = std::max(d, std::abs(output[index] - other.output[index]));
	}

	return d;
}

void fft_test_fftw::create_sine(double omega)
{
	for (size_t index = 0; index < size; ++index)
	{
		const double t = (double) index / (double) size;
		const double phi = t * omega;
		input[index] = sin(phi);
	}
}

void fft_test_fftw::transform()
{
	fftw_execute(plan);
}

void print(std::ostream & os, const size_t size, const double * data)
{
	for (size_t index = 0; index < size; ++index)
	{
		if (index > 0)
		{
			os << " ";
		}
		os << data[index];
	}
}

std::ostream & operator <<(std::ostream & os, const fft_test_fftw & x)
{
	os << std::fixed << std::setprecision(9);
	os << "size " << x.size << "\n";
	os << "input ";
	print(os, x.size, x.input);
	os << "\n";
	os << "output ";
	print(os, x.size, x.output);
	os << "\n";
	return os;
}

std::istream & operator >>(std::istream & is, fft_test_fftw & x)
{
	std::string keyword;
	size_t size = 0;

	is >> keyword;
	if (keyword != "size")
	{
		throw std::runtime_error("size keyword expected");
	}
	is >> size;

	x.create(size);

	is >> keyword;
	if (keyword != "input")
	{
		throw std::runtime_error("input keyword expected");
	}

	for (size_t index = 0; index < size; ++index)
	{
		double value;
		is >> value;
		x.input[index] = value;
	}

	is >> keyword;
	if (keyword != "output")
	{
		throw std::runtime_error("input keyword expected");
	}

	for (size_t index = 0; index < size; ++index)
	{
		double value;
		is >> value;
		x.output[index] = value;
	}

	return is;
}

void write_data(size_t size, double omega, const std::string &filename)
{
	fft_test_fftw test(size);
	test.create_sine(omega);
	test.transform();

	std::ofstream os(filename);

	os << test;
}

void test_roundtrip(size_t size, double omega)
{
	fft_test_fftw original(size);
	original.create_sine(omega);
	original.transform();

	std::stringstream ss;

	ss << original;

	fft_test_fftw copy;

	ss >> copy;

	double distance = original.distance(copy);

	if (!(distance < 1e-8))
	{
		throw std::runtime_error("copy does not match original");
	}
}

int main(const char *argv, int argc)
{
	test_roundtrip(16, 2 * M_PI);
	test_roundtrip(32, 4 * M_PI);
	test_roundtrip(64, M_E * M_PI);

	write_data(16, 2 * M_PI, "fft16.txt");
	write_data(32, 4 * M_PI, "fft32.txt");
	write_data(64, M_E * M_PI, "fft64.txt");

	return 0;
}

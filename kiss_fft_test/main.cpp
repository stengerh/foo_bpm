#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <fstream>

#define kiss_fft_scalar double
#include "kiss_fft/kiss_fft.h"

template<class T>
struct kiss_fft_allocator : public std::allocator<T>
{
	pointer allocate(size_type n, typename kiss_fft_allocator<T>::const_pointer hint = 0)
	{
		pointer p = KISS_FFT_MALLOC(n);
		if (p == 0)
		{
			throw std::bad_alloc();
		}
		return p;
	}

	void deallocator(pointer p, size_type n)
	{
		KISS_FFT_FREE(p);
	}
};

struct fft_test_kiss_fft
{
	typedef std::vector<double, kiss_fft_allocator<double> > scalar_buffer_type;
	typedef std::vector<kiss_fft_cpx, kiss_fft_allocator<kiss_fft_cpx> > complex_buffer_type;
	size_t size;
	scalar_buffer_type input;
	scalar_buffer_type expected_output;
	scalar_buffer_type actual_output;

	fft_test_kiss_fft();
	fft_test_kiss_fft(size_t size);
	~fft_test_kiss_fft();
	void create(size_t size);
	void transform();
	double distance() const;
};

void print(std::ostream & os, const fft_test_kiss_fft::scalar_buffer_type & data)
{
	os << std::fixed << std::setprecision(9);
	for (auto it = data.begin(); it != data.end(); ++it)
	{
		if (it != data.begin())
		{
			os << " ";
		}
		os << *it;
	}
}

void print(std::ostream & os, const fft_test_kiss_fft::complex_buffer_type & data)
{
	os << std::fixed << std::setprecision(9);
	for (auto it = data.begin(); it != data.end(); ++it)
	{
		if (it != data.begin())
		{
			os << " ";
		}
		os << it->r << "+" << it->i << "i";
	}
}

fft_test_kiss_fft::fft_test_kiss_fft():
	size(0)
{
}

fft_test_kiss_fft::fft_test_kiss_fft(size_t size):
	size(0)
{
	create(size);
}

fft_test_kiss_fft::~fft_test_kiss_fft()
{
}

void fft_test_kiss_fft::create(size_t size)
{
	this->size = size;
	input.resize(size);
	expected_output.resize(size);
	actual_output.resize(size);
}

void fft_test_kiss_fft::transform()
{
	kiss_fft_cfg cfg = kiss_fft_alloc(size, 0, (void *) 0, (size_t *) 0);

	complex_buffer_type cpx_input(size);
	complex_buffer_type cpx_output(size);

	for (size_t k = 0; k < size; ++k)
	{
		cpx_input[k].r = input[k];
		cpx_input[k].i = 0.0;
	}

	//std::cout << "[debug] complex_input "; print(std::cout, cpx_input); std::cout << std::endl;

	kiss_fft(cfg, &cpx_input[0], &cpx_output[0]);

	//std::cout << "[debug] complex_output "; print(std::cout, cpx_output); std::cout << std::endl;

	for (size_t k = 0; k <= size / 2; ++k)
	{
		actual_output[k] = cpx_output[k].r;
	}
	
	for (size_t k = 1; k < (size + 1) / 2; ++k)
	{
		actual_output[size - k] = cpx_output[k].i;
	}

	kiss_fft_free(cfg);
}

double fft_test_kiss_fft::distance() const
{
	double d = 0.0;

	for (size_t k = 0; k < size; ++k)
	{
		d = std::max(d, std::abs(expected_output[k] - actual_output[k]));
	}

	return d;
}

std::ostream & operator <<(std::ostream & os, const fft_test_kiss_fft & test)
{
	os << "size " << test.size << "\n";
	os << "input ";
	print(os, test.input);
	os << "\n";
	os << "expected_output ";
	print(os, test.expected_output);
	os << "\n";
	os << "actual_output   ";
	print(os, test.actual_output);
	os << "\n";
	os << "distance " << test.distance() << "\n";
	return os;
}

std::istream & operator >>(std::istream & is, fft_test_kiss_fft & test)
{
	std::string keyword;

	is >> keyword;
	if (keyword != "size")
	{
		throw std::runtime_error("expected size keyword");
	}

	size_t size;

	is >> size;

	test.create(size);

	is >> keyword;
	if (keyword != "input")
	{
		throw std::runtime_error("expected size keyword");
	}

	for (size_t index = 0; index < size; ++index)
	{
		double value;
		is >> value;
		test.input[index] = value;
	}

	is >> keyword;
	if (keyword != "output")
	{
		throw std::runtime_error("expected size keyword");
	}

	for (size_t index = 0; index < size; ++index)
	{
		double value;
		is >> value;
		test.expected_output[index] = value;
	}

	return is;
}

bool run_test(const std::string filename)
{
	fft_test_kiss_fft test;

	std::ifstream is(filename);
	std::ofstream os("log-" + filename);

	is >> test;

	test.transform();

	os << test;

	const double threshold = 1e-6;

	if (!(test.distance() < threshold))
	{
		os << "indices";

		for (size_t k = 0; k < test.size; ++k)
		{
			if (!(std::abs(test.expected_output[k] - test.actual_output[k]) < threshold))
			{
				os << "  " << k;
			}
		}

		os << "\n";
	}

	return test.distance() < threshold;
}

int main(const char *argv, int argc)
{
	run_test("fft16.txt");
	run_test("fft32.txt");
	run_test("fft64.txt");

	std::cout << "Press Return to continue..." << std::endl;
	char dummy;
	std::cin >> std::noskipws >> dummy;

	return 0;
}

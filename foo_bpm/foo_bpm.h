#ifndef __FOO_BPM__
#define __FOO_BPM__

// Standard C++ includes
#include <cmath>
#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>

// foo_bpm includes
#include "resource.h"
#include "globals.h"
#include "guid.h"
#include "preferences.h"

// foobar2000/FFTW includes
#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/helpers/helpers.h"
#include "fftw3.h"

using namespace std;

// Generate eXtra debug info
//#define X_DEBUG
//#define DEBUG
//#define CONSOLE

#ifdef X_DEBUG
const string audio_path = "d:\\foo_bpm_DEBUG_audio.txt";
const string stft_path = "d:\\foo_bpm_DEBUG_stft.txt";
const string flux_path = "d:\\foo_bpm_DEBUG_flux.txt";
const string peak_path = "d:\\foo_bpm_DEBUG_peak.txt";
const string bpm_path = "d:\\foo_bpm_DEBUG_bpm.txt";
#endif

// Math constants
const double M_PI = 3.14159265358979323846;
const double M_E = 2.71828182845904523536;

inline double sinc(double x) {return sin(M_PI*x)/(M_PI*x);}
// Windowing function definitions ([3])
inline double window_hamming(double sample_num, double fft_window_size) {return (0.54 - 0.46*cos((2*M_PI*sample_num)/(fft_window_size-1)));}
inline double window_hanning(double sample_num, double fft_window_size) {return (0.5 * (1 - cos((2*M_PI*sample_num)/(fft_window_size-1))));}
inline double window_cosine(double sample_num, double fft_window_size) {return (sin(M_PI*sample_num)/(fft_window_size-1));}
inline double window_lanczos(double sample_num, double fft_window_size) {return (sinc((2*sample_num)/(fft_window_size-1) - 1));}
inline double window_bartlett(double sample_num, double fft_window_size) {return ((2/(fft_window_size-1)) * ((fft_window_size-1)/2 - abs(sample_num-(fft_window_size-1)/2)));}
inline double window_triangular(double sample_num, double fft_window_size) {return ((2/(fft_window_size)) * ((fft_window_size)/2 - abs(sample_num-(fft_window_size-1)/2)));}
inline double window_gauss(double sample_num, double fft_window_size) {return pow(M_E, (-0.5*pow((((sample_num-(fft_window_size-1))/2) / ((0.4*(fft_window_size-1)/2))),2)));}

inline double rms(double* x, int size)
{
	double temp_rms = 0;

	for (int i = 0; i < size; i++)
	{
		temp_rms += x[i]*x[i];
	}

	return sqrt(temp_rms/size);
}

inline void bubble_sort(double* x, int size)
{
	bool swapped = false;

	do
	{
		swapped = false;
		for (int i = 0; i < size-1; i++)
		{
			if (x[i] > x[i+1])
			{
				swap(x[i], x[i+1]);
				swapped = true;
			}
		}
	} while (swapped);
}

inline vector<double> bubble_sort(vector<double> x)
{
	bool swapped = false;

	do
	{
		swapped = false;
		for (unsigned i = 0; i < x.size()-1; i++)
		{
			if (x[i] > x[i+1])
			{
				swap(x[i], x[i+1]);
				swapped = true;
			}
		}
	} while (swapped);

	return x;
}

inline void initialise_array(double* x, int size)
{
	for (int i = 0; i < size; i++)
		x[i] = 0;
}

inline double mean(vector<double> mean_list)
{
	double sum = 0;
	int size = (int)mean_list.size();

	for (int i = 0; i < size; i++)
		sum += mean_list[i];
	
	return sum / size;
}

inline double mode(vector<double> mode_list, double tolerance)
{
	double mode = 0;

	if (mode_list.size() > 0)
	{
		int current_mode_length = 1;
		int max_mode_length = 1;
		double current_mode_sum = mode_list[0];
		double max_mode_sum = mode_list[0];

		for (unsigned i = 1; i < mode_list.size(); i++)
		{
			if (mode_list[i] < mode_list[i-1] + tolerance &&
				mode_list[i] > mode_list[i-1] - tolerance)
			{
				current_mode_length++;
				current_mode_sum += mode_list[i];

				if (current_mode_length > max_mode_length)
				{
					max_mode_length = current_mode_length;
					max_mode_sum = current_mode_sum;
				}
			}
			else
			{
				current_mode_length = 1;
				current_mode_sum = mode_list[i];
			}
		}

		mode = max_mode_sum / (double)max_mode_length;
	}

	return mode;
}

#endif
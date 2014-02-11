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
#include "dialog.h"
#include "context_menu.h"
#include "preferences.h"

// foobar2000/FFTW includes
#include "../SDK/foobar2000.h"
#include "../helpers/helpers.h"
#include "../../../inc/fftw3.h"

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


class bpm_auto_analysis
{
	public:
		// Configurable BPM analysis parameters
		int seconds_to_read;
		int num_bpms_to_calc;
		double offset_pct_min;
		double offset_pct_max;

		// Configurable STFT parameters
		int fft_window_size;
		int fft_window_slide;
		int window_type;

		// Configurable tempo estimation parameters
		double bpm_min;
		double bpm_max;
		int bpm_precision;
		bool interpolate_flux;
		int candidate_selection;

		bpm_auto_analysis(metadb_handle_ptr playlist_item);
		~bpm_auto_analysis();
		double run(threaded_process_status & thread_status, abort_callback & p_abort);

	private:
		metadb_handle_ptr p_item;
		vector<double> bpm_list;

		// File input
		input_helper input_file;
		service_ptr_t<file> m_file;
		abort_callback_impl abort;
		double offset_pct_inc;

		// Audio decoding
		unsigned sample_rate;
		unsigned channel_count;
		unsigned max_samples;
		audio_chunk_impl chunk;
		audio_sample *data_ptr;
		t_size sample_count;
		audio_sample sample_downmix;
		vector<audio_sample> audio_buffer;

		// STFT parameters
		int fft_bin_size;
		int num_fft_windows;
		int sample_window_offset;
		double seconds_in_fft;
		double* fft_in;
		double* fft_out;
		double** stft;
		double* spectral_flux;
		fftw_plan fft_plan;

		// Peak detection
		bool do_peak_picking;
		double flux_constant;
		double flux_rms;
		int median_window_size;
		int actual_median_window_size;
		double* median_window;
		double* peaks;

		// Tempo estimation
		int tempo_comparisons;
		double tempo_offset_size;
		double tempo_offset_multiple;
		int offset_floor;
		int offset_ceiling;
		double offset_actual;
		double interpolated_peak;
		double sum;
		double max_sum;
		double estimated_bpm;
		#ifdef X_DEBUG
		double offset_sum;
		#endif

		bool read_file(double offset_pct);
		void calc_stft();
		void calc_spectral_flux();
		void pick_peaks();
		void calc_bpm();
		void clean_up();
};


class bpm_auto_analysis_thread : public threaded_process_callback
{
	public:
		bpm_auto_analysis_thread(metadb_handle_list_cref playlist_items);
		void start();

	private:
		pfc::list_t<metadb_handle_ptr> p_data;
		pfc::list_t<file_info_impl> p_info;
		int p_data_size;
		vector<double> bpm_results;
		void run(threaded_process_status & p_status, abort_callback & p_abort);
		void on_done(HWND p_wnd, bool p_was_aborted);
};

#endif
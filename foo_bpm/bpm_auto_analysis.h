#ifndef __BPM_AUTO_ANALYSIS_H__
#define __BPM_AUTO_ANALYSIS_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/helpers/helpers.h"

#include "bpm_fft.h"

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

		bpm_auto_analysis(metadb_handle_ptr p_track);
		~bpm_auto_analysis();
		double run_safe(threaded_process_status & thread_status, abort_callback & p_abort);

	private:
		metadb_handle_ptr m_track;
		std::vector<double> bpm_list;

		// File input
		input_helper input_file;
		service_ptr_t<file> m_file;
		double offset_pct_inc;

		// Audio decoding
		unsigned sample_rate;
		unsigned channel_count;
		unsigned max_samples;
		audio_chunk_impl chunk;
		audio_sample *data_ptr;
		t_size sample_count;
		audio_sample sample_downmix;
		std::vector<audio_sample> audio_buffer;

		// STFT parameters
		int fft_bin_size;
		int num_fft_windows;
		int sample_window_offset;
		double seconds_in_fft;
		//double* fft_in;
		//double* fft_out;
		double** stft;
		double* spectral_flux;
		bpm_fft::ptr m_fft;

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

 		double run(threaded_process_status & thread_status, abort_callback & p_abort);
		bool read_file(double offset_pct, abort_callback &p_abort);
		void calc_stft();
		void calc_spectral_flux();
		void pick_peaks();
		void calc_bpm();
		void clean_up();
};

#endif // __BPM_AUTO_ANALYSIS_H__

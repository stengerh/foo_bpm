/******************************************************************************
*          DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
*                    Version 2, December 2004
*
* Copyright (C) 2009-2010, Michael Balzer         Email: fraganator@hotmail.com
* Everyone is permitted to copy and distribute verbatim or modified
* copies of this license document, and changing it is allowed as long
* as the name is changed.
*
*            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
*   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
*
*  0. You just DO WHAT THE FUCK YOU WANT TO.
*
* This program is free software. It comes without any warranty, to
* the extent permitted by applicable law. You can redistribute it
* and/or modify it under the terms of the Do What The Fuck You Want
* To Public License, Version 2, as published by Sam Hocevar. See
* http://sam.zoy.org/wtfpl/COPYING for more details.
*
* If the code is redistributed or modified, please drop me an email letting me
* know how the code is being used. It's nice to know where it ends up :)
*
*******************************************************************************
*
* foo_bpm.cpp - A foobar2000 component for automatically detecting a song's BPM
*
* REFERENCES:
* BPM estimation algorithm based on:
* Tempo and Beat Estimation of Musical Signals - http://ismir2004.ismir.net/proceedings/p032-page-158-paper191.pdf
*
* OTHER REFERENCES:
* [1] Onset Detection Revisited - http://www.dafx.ca/proceedings/papers/p_133.pdf
* [2] A Comparison of Sound Onset Detection Algorithms with Emphasis on Psychoacoustically Motivated Detection Functions - http://www.cogs.susx.ac.uk/users/nc81/research/comparison.pdf
* [3] Window Functions - http://en.wikipedia.org/wiki/Window_function#Window_examples
*
* REVISION HISTORY:
* Date       | Version      | Description
* -----------------------------------------------------------------------------
* 21/12/2009 | 0.1.0        | Initial release
* -----------------------------------------------------------------------------
* 24/12/2009 | 0.1.1        | Refactored code (with processing time decrease)
*            |              | Initial source code release
* -----------------------------------------------------------------------------
* 31/12/2009 | 0.2.0        | Added confirmation to rescan already tagged files
*            |              | Added ReplayGain style results dialog
*            |              | Added preferences page (with destination BPM tag)
*            |              | Added manual bpm calculation window
* -----------------------------------------------------------------------------
* 02/01/2010 | 0.2.1        | Miscellaneous bug fixes
* -----------------------------------------------------------------------------
* 07/01/2010 | 0.2.2        | Crash report fix
*            |              | Candidate bpm selection can be mode, mean, median
*            |              | added to preferences
*            |              | Debug output added to preferences
* -----------------------------------------------------------------------------
* 11/01/2010 | 0.2.3        | Updated to foobar2000 1.0 SDK
*            |              | Added double/halve buttons to results dialog
*            |              | Added option to auto write tags after analysis
*            |              | Limit preference range inputs
*            |              | Crash report fix (using info not yet cached)
* -----------------------------------------------------------------------------
* 12/01/2010 | 0.2.4        | Crash report fix (component about message)
* -----------------------------------------------------------------------------
* 19/04/2010 | 0.2.4.1      | Numerous bug fixes
*
******************************************************************************/

#include "stdafx.h"
#include "foo_bpm.h"

DECLARE_COMPONENT_VERSION("BPM Analyser", "0.2.4.1",
"A component for automatically analysing the BPM of audio files.\n\n\
Makes use of the FFTW library (www.fftw.org)\n\n\
References:\n\
Tempo and Beat Estimation of Musical Signals -\nhttp://ismir2004.ismir.net/proceedings/p032-page-158-paper191.pdf \n\n\
Onset Detection Revisited -\nhttp://www.dafx.ca/proceedings/papers/p_133.pdf \n\n\
A Comparison of Sound Onset Detection Algorithms with Emphasis on Psychoacoustically Motivated Detection Functions -\nhttp://www.cogs.susx.ac.uk/users/nc81/research/comparison.pdf \n\n\
Window Functions -\nhttp://en.wikipedia.org/wiki/Window_function#Window_examples \n\n\n\
(c) 2009-2010 Michael Balzer (fraganator@hotmail.com)");

VALIDATE_COMPONENT_FILENAME("foo_bpm.dll");

/***** BPM Analysis *****/

bpm_auto_analysis::bpm_auto_analysis(metadb_handle_ptr playlist_item)
{
	p_item = playlist_item;

	seconds_to_read = bpm_config_seconds_to_read;
	num_bpms_to_calc = bpm_config_num_bpms_to_calc;
	offset_pct_min = (double)bpm_config_offset_pct_min / 100.0;
	offset_pct_max = (double)bpm_config_offset_pct_max / 100.0;
	// Do some checks to make sure the input settings are OK
	if (offset_pct_min > offset_pct_max)
	{
		swap(offset_pct_min, offset_pct_max);
	}
	else if (offset_pct_min == offset_pct_max)
	{
		offset_pct_min--;
	}
	offset_pct_inc = (offset_pct_max - offset_pct_min) / (double)num_bpms_to_calc;

	sample_rate = 0;
	channel_count = 0;
	max_samples = 0;
	sample_count = 0;
	sample_downmix = 0;

	// Starting value and order of enum make this possible. Saves us from a big switch statement.
	fft_window_size = 64 * (int)pow(2, (double)bpm_config_fft_window_size);
	fft_window_slide = 64 * (int)pow(2, (double)bpm_config_fft_window_slide);
	fft_bin_size = fft_window_size/2 + 1;
	num_fft_windows = 0;
	sample_window_offset = 0;
	seconds_in_fft = 0;
	sample_count = 0;
	sample_downmix = 0;
	window_type = bpm_config_fft_window_type;

	do_peak_picking = false;
	flux_constant = 2;
	flux_rms = 0;
	median_window_size = fft_window_size/25;
	actual_median_window_size = median_window_size;

	bpm_min = bpm_config_bpm_min;
	bpm_max = bpm_config_bpm_max;
	// Do some checks to make sure the input settings are OK
	if (bpm_min > bpm_max)
	{
		swap(bpm_min, bpm_max);
	}
	else if (bpm_min == bpm_max)
	{
		bpm_max++;
	}
	bpm_precision = bpm_config_bpm_precision;
	tempo_comparisons = 0;
	tempo_offset_size = 0;
	tempo_offset_multiple = 1;
	offset_floor = 0;
	offset_ceiling = 0;
	offset_actual = 0;
	interpolate_flux = bpm_config_interpolate_flux;
	interpolated_peak = 0;
	sum = 0;
	max_sum = 0;
	estimated_bpm = 0;
	#ifdef X_DEBUG
	offset_sum = 0;
	#endif
	candidate_selection = bpm_config_candidate_selection;
}

bpm_auto_analysis::~bpm_auto_analysis()
{

}

double bpm_auto_analysis::run(threaded_process_status & thread_status, abort_callback & p_abort)
{
	double bpm_result = 0;
	int progress = 0;
	int max_progress = (int)((offset_pct_max - offset_pct_min) / offset_pct_inc) * 6; // Multiply by 6 due to the 6 steps in the loop below

	thread_status.set_progress(0, max_progress);

	if (p_item->get_length() < seconds_to_read)
	{
		thread_status.set_progress(max_progress, max_progress);
		console::formatter() << "foo_bpm: Error analysing " << p_item->get_path() << ". Track length too short. BPM set to zero.";
		return 0;
	}

	for (double offset_pct = offset_pct_min; offset_pct < offset_pct_max; offset_pct+=offset_pct_inc)
	{
		// If we got an error back, skip this file's analysis. It'll be missing important values such as sample rate which are needed. BPM is set to zero.
		if (!read_file(offset_pct))
		{
			bpm_list.push_back(0);
			break;
		}
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
		calc_stft();
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
		calc_spectral_flux();
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
		pick_peaks();
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
		calc_bpm();
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
		clean_up();
		thread_status.set_progress(++progress, max_progress);
		if (p_abort.is_aborting()) break;
	}

	bpm_list = bubble_sort(bpm_list);

	switch (candidate_selection)
	{
		case BPM_MEAN:
			bpm_result = mean(bpm_list);
			break;
		case BPM_MODE:
			bpm_result = mode(bpm_list, 0.8);
			break;
		case BPM_MEDIAN:
		default:
			bpm_result = bpm_list[bpm_list.size()/2];
			break;
	}

	if (bpm_config_output_debug)
	{
		console::formatter() << "\n";
		console::formatter() << "foo_bpm: Estimated BPMs (sorted) for " << pfc::string_filename_ext(p_item->get_path()) << " are:";
		for (unsigned i = 0; i < bpm_list.size(); i++)
			console::formatter() << "BPM " << i+1 << " = " << bpm_list[i];

		console::formatter() << "foo_bpm: Calculated BPM = " << bpm_result;
		console::formatter() << "\n";
	}

	return bpm_result;
}

bool bpm_auto_analysis::read_file(double offset_pct)
{
	if (!input_file.is_open())
	{
		// Open the file if it isn't already open
		input_file.open(m_file, p_item, 0, abort, false, false);
	}

	if (!input_file.is_open())
	{
		console::formatter() << "foo_bpm: Error analysing " << p_item->get_path() << ". File could not be opened for analysis";
		return false;
	}

	if (input_file.can_seek())
	{
		input_file.seek(p_item->get_length()*offset_pct, abort);
	}
	else
	{
		console::formatter() << "foo_bpm: Warning - Failed to seek file " << p_item->get_path() << ". BPM result will be of first " << seconds_to_read << " seconds only.";
	}

	// Grab and ignore the first chunk as it will most likely have an odd sample chunk size
	// TODO: Handle case where we reach EOF (run() returns false)
	if (!input_file.run(chunk, abort))
	{
		console::formatter() << "foo_bpm: Error analysing " << p_item->get_path() << ". Unexpected end of file found.";
		return false;
	}

	sample_rate = chunk.get_srate();
	channel_count = chunk.get_channels();

	max_samples = sample_rate*seconds_to_read;

	#ifdef DEBUG
	console::formatter() << "\n\n\nFile path: " << input_file.get_path();
	console::formatter() << "Sample rate: " << sample_rate << "Hz";
	console::formatter() << "Num channels: " << channel_count;
	#endif

	// Reset the audio buffer before filling it
	audio_buffer.resize(0);

	while (audio_buffer.size() < max_samples)
	{
		// Get a chunk of audio from the file so we can extract sample/channel properties
		input_file.run(chunk, abort);

		// Sample is a unit of interleaved PCM data (ie 1 sample corresponds to 2 values for stereo PCM data, 6 for 5.1 etc)
		sample_count = chunk.get_sample_count();
		data_ptr = chunk.get_data();

		#ifdef X_DEBUG
		console::formatter() << "Chunk size: " << sample_count;
		#endif

		// Increase buffer size by sample_count, but only if necessary
		if (audio_buffer.capacity() <= audio_buffer.size() + sample_count)
		{
			audio_buffer.reserve(audio_buffer.size() + sample_count);
		}

		// For each sample in the audio chunk
		for (t_size sample_num = 0; sample_num < sample_count; sample_num++)
		{
			sample_downmix = 0;

			// For each of the interleaved channels
			for (unsigned channel_num = 0; channel_num < channel_count; channel_num++)
			{
				sample_downmix += data_ptr[0];
				data_ptr++;
			}

			// Take average of summed channel data to get single sample
			sample_downmix = sample_downmix / channel_count;

			audio_buffer.push_back(sample_downmix);
		}
	}

	#ifdef DEBUG
	console::formatter() << "Buffer size is " << audio_buffer.size();
	console::formatter() << "Allocated size is " << audio_buffer.capacity();
	#endif

	return true;
}

void bpm_auto_analysis::calc_stft()
{
	if (bpm_config_fft_window_slide == FFT_SLIDE_AUTO)
	{
		// Human hearing can only determine difference of 10ms ([1])
		fft_window_slide = sample_rate/100;
	}
	// Work out the number of sliding windows available
	num_fft_windows = (int)((audio_buffer.size() - fft_window_size) / fft_window_slide) + 1; // BUG: Divide by 0 - #24710, #24506. My hunch is playlist info is not yet available, so sample_rate has a value of zero.
	// Work out number of seconds we're actually processing
	seconds_in_fft = (double)(fft_window_slide * (num_fft_windows - 1) + fft_window_size) / (double)sample_rate;
	fft_in = new double[fft_window_size];
	fft_out = new double[fft_window_size];
	// stft is a 2D array of num_fft_windows by fft_bin_size
	stft = new double*[num_fft_windows];
	// Output array will be out[r0,r1,r2,...,rn/2, i(n+1)/2-1,...,i2,i1]
	fft_plan = fftw_plan_r2r_1d(fft_window_size, fft_in, fft_out, FFTW_R2HC, FFTW_MEASURE);

	// Window will slide along buffer
	for (int fft_window = 0; fft_window < num_fft_windows; fft_window++)
	{
		// Current offset into audio buffer
		sample_window_offset = fft_window*fft_window_slide;

		// Copy from the audio buffer an FFT window sized array for FFT processing
		for (int sample_num = 0; sample_num < fft_window_size; sample_num++)
		{
			switch (window_type)
			{
				case FFT_WINDOW_HAMMING:
					fft_in[sample_num] = (double)audio_buffer[sample_window_offset + sample_num] * window_hamming(sample_num, fft_window_size);
					break;
				case FFT_WINDOW_HANNING:
					fft_in[sample_num] = (double)audio_buffer[sample_window_offset + sample_num] * window_hanning(sample_num, fft_window_size);
					break;
				case FFT_WINDOW_GAUSIAN:
					fft_in[sample_num] = (double)audio_buffer[sample_window_offset + sample_num] * window_gauss(sample_num, fft_window_size);
					break;
				case FFT_WINDOW_RECT:
				default:
					break;
			}
		}

		// Do the FFT
		fftw_execute(fft_plan);

		stft[fft_window] = new double[fft_bin_size];
		// Copy the FFT result into the 2D STFT array
		for (int fft_bin = 0; fft_bin < fft_bin_size; fft_bin++)
		{
			stft[fft_window][fft_bin] = abs(fft_out[fft_bin]);
			// TODO: Make the fft result processing configurable
//			stft[fft_window][fft_bin] = fft_out[fft_bin];
//			stft[fft_window][fft_bin] = pow(fft_out[fft_bin],2);
//			stft[fft_window][fft_bin] = pow(fft_out[fft_bin],2)*fft_bin;
		}
	}

	#ifdef X_DEBUG
	ofstream audio_file;
	audio_file.open(audio_path.c_str(), ios::out);
	for (unsigned i = 0; i < audio_buffer.size(); i++)
	{
		audio_file << audio_buffer[i];
		if (i != audio_buffer.size() - 1)
			audio_file << "\t";
	}
	audio_file.close();

	ofstream stft_file;
	stft_file.open(stft_path.c_str(), ios::out);
	for (int i = 0; i < num_fft_windows; i++)
	{
		for (int j = 0; j < fft_bin_size; j++)
		{
			stft_file << stft[i][j];
			if (j != fft_bin_size - 1)
				stft_file << "\t";
		}
		stft_file << "\n";
	}
	stft_file.close();
	#endif
}

void bpm_auto_analysis::calc_spectral_flux()
{
	spectral_flux = new double[num_fft_windows];
	initialise_array(spectral_flux, num_fft_windows);

	for (int fft_window = 0; fft_window < num_fft_windows; fft_window++)
	{
		for (int fft_bin = 0; fft_bin < fft_bin_size; fft_bin++)
		{
			if (fft_window > 0)
			{
				if (stft[fft_window][fft_bin] > stft[fft_window-1][fft_bin])
				{
					// TODO: Make the spectral weight configurable
					// No weighting
//					spectral_flux[fft_window-1] += stft[fft_window][fft_bin];
					// Difference
//					spectral_flux[fft_window-1] += stft[fft_window][fft_bin] - stft[fft_window-1][fft_bin];
//					spectral_flux[fft_window-1] += pow(stft[fft_window][fft_bin] - stft[fft_window-1][fft_bin], 2);
					// Multiply by fft_bin to apply weight to higher frequencies ([2])
					// I've found this weighting to give the best results, but others may perform better for different genres
					spectral_flux[fft_window-1] += pow(stft[fft_window][fft_bin] - stft[fft_window-1][fft_bin], 2) * fft_bin;
					// Weight formula variation ([2])
//					spectral_flux[fft_window-1] += (stft[fft_window][fft_bin] - stft[fft_window-1][fft_bin]) * fft_bin * fft_bin;
					// Approximate pshychoacoustic weighting ([2])
//					spectral_flux[fft_window-1] += log(stft[fft_window][fft_bin] / stft[fft_window-1][fft_bin]);
				}
			}
			else
			{
				// Assume difference of fft_bins at fft_windows -1 and 0 is 0 (no change in energy)
				spectral_flux[fft_window] = 0;
			}
		}

		// Only necessary if stft isn't absolute values (see post FFT processing in calc_stft())
		//spectral_flux[fft_window-1] = (spectral_flux[fft_window-1] + abs(spectral_flux[fft_window-1]))/2;
	}

	#ifdef X_DEBUG
	ofstream flux_file;
	flux_file.open(flux_path.c_str(), ios::out);
	for (int i = 0; i < num_fft_windows; i++)
	{
		flux_file << spectral_flux[i];
		if (i != num_fft_windows - 1)
			flux_file << "\t";
	}
	flux_file.close();
	#endif
}

void bpm_auto_analysis::pick_peaks()
{
	median_window = new double[median_window_size];
	peaks = new double[num_fft_windows];
	initialise_array(peaks, num_fft_windows);

	if (do_peak_picking)
	{
		// Use a sliding window across the flux output to obtain a subset of values
		// Sort the subset, then grab the median value. If the current flux value exceeds
		// the the median multiplied by some scaling factor, summed with the rms of the
		// flux, consider it a peak (or beat). Otherwise it will be zero.
		for (int fft_window = 0; fft_window < num_fft_windows; fft_window++)
		{
			// Case where median window would exceed lower range of flux array
			if (fft_window < median_window_size/2)
			{
				actual_median_window_size = fft_window + median_window_size/2;

				for (int i = 0; i < actual_median_window_size; i++)
				{
					median_window[i] = spectral_flux[i];
				}
			}
			// Case where median window would exceed upper range of flux array
			else if (fft_window > num_fft_windows - median_window_size/2)
			{
				actual_median_window_size = num_fft_windows - fft_window + median_window_size/2;

				for (int i = 0; i < actual_median_window_size; i++)
				{
					median_window[i] = spectral_flux[fft_window-median_window_size/2+i];
				}
			}
			// Median window won't exceed ends of array
			else
			{
				actual_median_window_size = median_window_size;

				for (int i = 0; i < actual_median_window_size; i++)
				{
					median_window[i] = spectral_flux[fft_window-(median_window_size/2)+i];
				}
			}
			
			bubble_sort(median_window, actual_median_window_size);

			if (spectral_flux[fft_window] > (median_window[actual_median_window_size/2] * flux_constant + flux_rms))
				peaks[fft_window] = spectral_flux[fft_window];
		}

		#ifdef X_DEBUG
		ofstream peak_file;
		peak_file.open(peak_path.c_str(), ios::out);
		for (int i = 0; i < num_fft_windows; i++)
		{
			peak_file << peaks[i];
			if (i != num_fft_windows - 1)
				peak_file << "\t";
		}
		peak_file.close();
		#endif
	}
}

void bpm_auto_analysis::calc_bpm()
{
	double tempo_inc = 1;
	max_sum = 0;

	switch (bpm_precision)
	{
		case BPM_PRECISION_1:
			tempo_inc = 1; break;
		case BPM_PRECISION_1DP:
			tempo_inc = 0.1; break;
		case BPM_PRECISION_2DP:
			tempo_inc = 0.01; break;
		default:
			tempo_inc = 1; break;
	}

	for (double tempo = bpm_min; tempo <= bpm_max; tempo+=tempo_inc)
	{
		tempo_offset_size = (double)num_fft_windows * (60/tempo) / seconds_in_fft;
		tempo_comparisons = (int)tempo_offset_size;
		#ifdef X_DEBUG
		offset_sum = 0;
		#endif

		for (int tempo_offset = 0; tempo_offset < tempo_comparisons; tempo_offset++)
		{
			sum = 0;
			tempo_offset_multiple = 1;
			while (tempo_offset_size * tempo_offset_multiple + tempo_offset < num_fft_windows)
			{
				offset_actual = tempo_offset_size * tempo_offset_multiple + tempo_offset;
				offset_floor = (int)floor(offset_actual);
				offset_ceiling = (int)ceil(offset_actual);

				if (do_peak_picking)
				{
					if (interpolate_flux && (offset_ceiling > offset_floor))
					{
						interpolated_peak = peaks[offset_floor]*(offset_ceiling - offset_actual) + peaks[offset_ceiling]*(offset_actual - offset_floor);
					}
					else
					{
						interpolated_peak = peaks[offset_floor];
					}
				}
				else
				{
					if (interpolate_flux && (offset_ceiling > offset_floor))
					{
						interpolated_peak = spectral_flux[offset_floor]*(offset_ceiling - offset_actual) + spectral_flux[offset_ceiling]*(offset_actual - offset_floor);
					}
					else
					{
						interpolated_peak = spectral_flux[offset_floor];
					}
				}

				sum += interpolated_peak + 100000;

				tempo_offset_multiple++;
			}

			sum = sum / (tempo_offset_multiple - 1);

			if (sum > max_sum)
			{
				max_sum = sum;
				estimated_bpm = tempo;
			}
			// Check to see if the tempo may be double the current tempo by checking if the sum is greater than 90% of the current max sum
			// TODO: Be a little more scientific about the 90% value. ie some scaled percentage based on tempo
			else if (tempo > estimated_bpm*2 - 2 &&
					 tempo < estimated_bpm*2 + 2 &&
					 sum > max_sum*0.9)
			{
				max_sum = sum;
				estimated_bpm = tempo;
			}

			#ifdef X_DEBUG
			if (sum > offset_sum)
				offset_sum = sum;
			#endif
		}

		#ifdef X_DEBUG
		console::formatter() << "Tested BPM: " << tempo << ", sum is " << offset_sum << ", max sum is " << max_sum;
		#endif
	}

	bpm_list.push_back(estimated_bpm);

	#ifdef DEBUG
	console::formatter() << "Num FFT windows: " << num_fft_windows;
	console::formatter() << "Seconds in FFT: " << seconds_in_fft;
	#endif

	#ifdef X_DEBUG
	ofstream bpm_file;
	bpm_file.open(bpm_path.c_str(), ios::out);
	bpm_file << estimated_tempo;
	bpm_file.close();
	#endif
}

void bpm_auto_analysis::clean_up()
{
	input_file.close();

	fftw_destroy_plan(fft_plan);
	delete[] fft_in;
	delete[] fft_out;
	for (int i = 0; i < num_fft_windows; i++)
	{
		delete[] stft[i];
	}
	delete[] stft;
	delete[] spectral_flux;
	delete[] median_window;
	delete[] peaks;
}










/***** Threading *****/

bpm_auto_analysis_thread::bpm_auto_analysis_thread(metadb_handle_list_cref playlist_items)
{
	p_data_size = playlist_items.get_size();
	p_data.set_size(p_data_size);
	p_info.set_size(p_data_size);
	for (int i = 0; i < p_data_size; i++)
	{
		p_data[i] = playlist_items[i];
	}
}

void bpm_auto_analysis_thread::start()
{
	bool bpm_missing = false;
	bool rescan = false;

	// For each item in the playlist selection
	for (int item = 0; item < p_data_size; item++)
    {
		if (!p_data[item]->get_info(p_info[item]))
		{
			p_data.remove_by_idx(item);
			p_info.remove_by_idx(item);
			p_data_size--;
			item--;
			continue;
		}
		
		// Are any of the tracks missing the BPM?
		if (!bpm_missing && p_info[item].meta_get(bpm_config_bpm_tag.get_ptr(), 0) == 0)
		{
			bpm_missing = true;
		}
	}

	if (bpm_missing)
	{
		// For each item in the playlist selection
		for (int item = 0; item < p_data_size; item++)
		{
			// Remove any items that already have a BPM set. We only want to scan BPM-less tracks
			if (p_info[item].meta_get(bpm_config_bpm_tag.get_ptr(), 0) != 0)
			{
				p_data.remove_by_idx(item);
				p_info.remove_by_idx(item);
				p_data_size--;
				item--;
			}
		}
	}
	else
	{
		int response = MessageBox(core_api::get_main_window(),
					_T("All tracks you have selected already have BPM info. Would you like to scan them anyway?"),
					_T("Information"),
					MB_YESNO);

		if (response == IDYES)
		{
			rescan = true;
		}
	}

	if (rescan || bpm_missing)
	{
		threaded_process::g_run_modeless( 
			this,
			threaded_process::flag_show_abort | 
			threaded_process::flag_show_delayed |
			threaded_process::flag_show_minimize |
			threaded_process::flag_show_progress_dual |
			threaded_process::flag_show_item,
			core_api::get_main_window(),
			"Analysing BPMs..."
			);
	}
}

void bpm_auto_analysis_thread::run(threaded_process_status & p_status, abort_callback & p_abort)
{
	bpm_results.resize(0);

	p_status.set_progress_secondary(0, p_data_size);

	// For each item in the playlist selection
	for (int item = 0; item < p_data_size; item++)
    {
		// Skip the file if it doesn't exist
		if (!filesystem::g_exists(p_data[item]->get_path(), p_abort))
		{
			p_data.remove_by_idx(item);
			p_info.remove_by_idx(item);
			p_data_size--;
			item--;
		}
		else
		{
			p_status.set_item_path(p_data[item]->get_location().get_path(), ~0);
			bpm_auto_analysis bpm(p_data[item]);
			bpm_results.push_back(bpm.run(p_status, p_abort));

			p_status.set_progress_secondary(item+1, p_data_size);
		}

		if (p_abort.is_aborting()) break;
	}
}

void bpm_auto_analysis_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (!p_was_aborted && core_api::assert_main_thread())
	{
		bpm_result_dialog* m_result_dialog = new bpm_result_dialog();
		m_result_dialog->p_data = p_data;
		m_result_dialog->p_info = p_info;
		m_result_dialog->p_data_size = p_data_size;
		m_result_dialog->bpm_results = bpm_results;

		m_result_dialog->Create(core_api::get_main_window(), NULL);
		m_result_dialog->ShowWindow(SW_SHOWNORMAL);
	}
}









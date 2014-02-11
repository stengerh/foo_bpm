#include "stdafx.h"
#include "preferences.h"

// General
cfg_int bpm_config_bpm_precision(guid_bpm_config_bpm_precision, BPM_PRECISION_1);
cfg_string bpm_config_bpm_tag(guid_bpm_config_bpm_tag, "BPM");
cfg_bool bpm_config_auto_write_tag(guid_bpm_config_auto_write_tag, false);
// Auto
cfg_int bpm_config_seconds_to_read(guid_bpm_config_seconds_to_read, 10);
cfg_int bpm_config_num_bpms_to_calc(guid_bpm_config_num_bpms_to_calc, 10);
cfg_int bpm_config_offset_pct_min(guid_bpm_config_offset_pct_min, 20);
cfg_int bpm_config_offset_pct_max(guid_bpm_config_offset_pct_max, 80);
cfg_int bpm_config_fft_window_size(guid_bpm_config_fft_window_size, FFT_SIZE_2048);
cfg_int bpm_config_fft_window_slide(guid_bpm_config_fft_window_slide, FFT_SLIDE_AUTO);
cfg_int bpm_config_fft_window_type(guid_bpm_config_window_type, FFT_WINDOW_HAMMING);
cfg_int bpm_config_bpm_min(guid_bpm_config_bpm_min, 75);
cfg_int bpm_config_bpm_max(guid_bpm_config_bpm_max, 195);
cfg_bool bpm_config_interpolate_flux(guid_bpm_config_interpolate_flux, true);
cfg_int bpm_config_candidate_selection(guid_bpm_config_candidate_selection, 0);
cfg_bool bpm_config_output_debug(guid_bpm_config_output_debug, false);
// Manual
cfg_int bpm_config_taps_to_average(guid_bpm_config_taps_to_average, 30);
cfg_int bpm_config_seconds_to_reset_average(guid_bpm_config_seconds_to_reset_average, 5);

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "../SDK/foobar2000.h"
#include "guid.h"

// Config variables
// General
extern cfg_int bpm_config_bpm_precision;
extern cfg_string bpm_config_bpm_tag;
extern cfg_bool bpm_config_auto_write_tag;
// Auto
extern cfg_int bpm_config_seconds_to_read;
extern cfg_int bpm_config_num_bpms_to_calc;
extern cfg_int bpm_config_offset_pct_min;
extern cfg_int bpm_config_offset_pct_max;
extern cfg_int bpm_config_fft_window_size;
extern cfg_int bpm_config_fft_window_slide;
extern cfg_int bpm_config_fft_window_type;
extern cfg_int bpm_config_bpm_min;
extern cfg_int bpm_config_bpm_max;
extern cfg_bool bpm_config_interpolate_flux;
extern cfg_int bpm_config_candidate_selection;
extern cfg_bool bpm_config_output_debug;
// Manual
extern cfg_int bpm_config_taps_to_average;
extern cfg_int bpm_config_seconds_to_reset_average;

#endif
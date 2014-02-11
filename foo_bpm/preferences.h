#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include "resource.h"
#include "globals.h"
#include "guid.h"

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

typedef enum
{
	BPM_PRECISION_1 = 0,
	BPM_PRECISION_1DP,
	BPM_PRECISION_2DP
} bpm_precision_enum;

typedef enum
{
	FFT_SIZE_64 = 0,
	FFT_SIZE_128,
	FFT_SIZE_256,
	FFT_SIZE_512,
	FFT_SIZE_1024,
	FFT_SIZE_2048,
	FFT_SIZE_4096,
	FFT_SIZE_8192
} fft_window_size_enum;

typedef enum
{
	FFT_SLIDE_64 = 0,
	FFT_SLIDE_128,
	FFT_SLIDE_256,
	FFT_SLIDE_512,
	FFT_SLIDE_1024,
	FFT_SLIDE_2048,
	FFT_SLIDE_4096,
	FFT_SLIDE_8192,
	FFT_SLIDE_AUTO
} fft_window_slide_enum;

typedef enum
{
	FFT_WINDOW_HAMMING = 0,
	FFT_WINDOW_HANNING,
	FFT_WINDOW_GAUSIAN,
	FFT_WINDOW_RECT
} fft_window_enum;

typedef enum
{
	BPM_MODE = 0,
	BPM_MEDIAN,
	BPM_MEAN
} bpm_selection;

#endif
#include "stdafx.h"
#include "bpm_fft.h"
#include "bpm_fft_impl_fftw.h"

bpm_fft::ptr bpm_fft::g_create()
{
	return new bpm_fft_impl_fftw();
}
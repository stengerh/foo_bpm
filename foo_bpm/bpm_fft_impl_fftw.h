#ifndef __BPM_FFT_IMPL_FFTW_H__
#define __BPM_FFT_IMPL_FFTW_H__

#include "foobar2000/shared/shared.h"

#include "bpm_fft.h"

#include "fftw3.h"

class releaser_fftw_free {
public:
	static void release(void * p_ptr) {fftw_free(p_ptr);}
};

class bpm_fft_impl_fftw : public bpm_fft
{
public:
	bpm_fft_impl_fftw();
	~bpm_fft_impl_fftw();

	void create_plan(int p_size);
	void execute_plan();
	void destroy_plan();
	double * get_input_buffer();
	double * get_output_buffer();

private:
	typedef pfc::ptrholder_t<double, releaser_fftw_free> buffer_ptr;

	fftw_plan m_plan;
	buffer_ptr m_input_buffer;
	buffer_ptr m_output_buffer;

	static critical_section2 g_plan_mutex;
};

#endif // __BPM_FFT_IMPL_FFTW_H__

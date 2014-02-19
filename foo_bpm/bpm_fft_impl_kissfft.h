#ifndef __BPM_FFT_IMPL_kissfft_H__
#define __BPM_FFT_IMPL_kissfft_H__

#include "foobar2000/shared/shared.h"

#include "bpm_fft.h"

#define kiss_fft_scalar double
#include "kiss_fft/kiss_fft.h"

class releaser_kissfft_free {
public:
	static void release(void * p_ptr) {free(p_ptr);}
};

class bpm_fft_impl_kissfft : public bpm_fft
{
public:
	bpm_fft_impl_kissfft();
	~bpm_fft_impl_kissfft();

	void create_plan(int p_size);
	void execute_plan();
	void destroy_plan();
	double * get_input_buffer();
	double * get_output_buffer();

private:
	typedef pfc::ptrholder_t<kiss_fft_scalar, releaser_kissfft_free> kiss_fft_scalar_ptr;
	typedef pfc::ptrholder_t<kiss_fft_cpx, pfc::releaser_free> kiss_fft_complex_ptr;
	typedef pfc::ptrholder_t<kiss_fft_state, pfc::releaser_free> kiss_fft_state_ptr;

	int m_size;
	kiss_fft_state_ptr m_plan;
	kiss_fft_scalar_ptr m_input_buffer;
	kiss_fft_scalar_ptr m_output_buffer;
	kiss_fft_complex_ptr m_complex_input_buffer;
	kiss_fft_complex_ptr m_complex_output_buffer;
};

#endif // __BPM_FFT_IMPL_kissfft_H__

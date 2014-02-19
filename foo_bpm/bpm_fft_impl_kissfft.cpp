
#include "bpm_fft_impl_kissfft.h"

bpm_fft_impl_kissfft::bpm_fft_impl_kissfft():
	m_plan(nullptr)
{
}

bpm_fft_impl_kissfft::~bpm_fft_impl_kissfft()
{
}

void bpm_fft_impl_kissfft::create_plan(int p_size)
{
	m_plan = nullptr;
	kiss_buffer = nullptr;

	m_plan = kiss_fftr_alloc(p_size, 0, 0, 0);
	fftlen = p_size;

	m_input_buffer = (double*)KISS_FFT_MALLOC(p_size*sizeof(double));
	m_output_buffer = (double*)KISS_FFT_MALLOC(p_size*sizeof(double));
    kiss_buffer=(kiss_fft_cpx*)KISS_FFT_MALLOC(p_size*sizeof(kiss_fft_cpx));

}

void bpm_fft_impl_kissfft::execute_plan()
{
	PFC_ASSERT(m_plan.is_valid());
	kiss_fftr(m_plan.get_ptr(), m_input_buffer.get_ptr(), kiss_buffer.get_ptr());
	double * ptr  = m_output_buffer.get_ptr();
	for (int i=0;i<fftlen;i++)
	{
		ptr[i] = (kiss_buffer.get_ptr()[i].r * kiss_buffer.get_ptr()[i].r) + (kiss_buffer.get_ptr()[i].i * kiss_buffer.get_ptr()[i].i);
	}

}

void bpm_fft_impl_kissfft::destroy_plan()
{
	m_plan.release();
	kiss_buffer.release();
	m_input_buffer.release();
	m_output_buffer.release();
}

double * bpm_fft_impl_kissfft::get_input_buffer()
{
	return m_input_buffer.get_ptr();
}

double * bpm_fft_impl_kissfft::get_output_buffer()
{
	return m_output_buffer.get_ptr();
}

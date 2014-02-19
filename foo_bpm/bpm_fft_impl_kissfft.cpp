
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

	m_plan = kiss_fft_alloc(p_size, 0, 0, 0);
	m_size = p_size;

	m_input_buffer = (double*)KISS_FFT_MALLOC(p_size*sizeof(double));
	m_output_buffer = (double*)KISS_FFT_MALLOC(p_size*sizeof(double));

	m_complex_input_buffer = (kiss_fft_cpx*)KISS_FFT_MALLOC(p_size*sizeof(kiss_fft_cpx));
	m_complex_output_buffer = (kiss_fft_cpx*)KISS_FFT_MALLOC(p_size*sizeof(kiss_fft_cpx));
}

void bpm_fft_impl_kissfft::execute_plan()
{
	PFC_ASSERT(m_plan.is_valid());

	for (int index = 0; index < m_size; ++index)
	{
		m_complex_input_buffer.get_ptr()[index].r = m_input_buffer.get_ptr()[index];
		m_complex_input_buffer.get_ptr()[index].i = 0.0;
	}

	kiss_fft(m_plan.get_ptr(), m_complex_input_buffer.get_ptr(), m_complex_output_buffer.get_ptr());

	for (int index = 0; index <= m_size / 2; ++index)
	{
		m_output_buffer.get_ptr()[index] = m_complex_output_buffer.get_ptr()[index].r;
	}

	for (int index = 1; index < (m_size + 1) / 2; ++index)
	{
		m_output_buffer.get_ptr()[m_size - index] = m_complex_output_buffer.get_ptr()[index].i;
	}
}

void bpm_fft_impl_kissfft::destroy_plan()
{
	m_plan.release();

	m_input_buffer.release();
	m_output_buffer.release();

	m_complex_input_buffer.release();
	m_complex_output_buffer.release();
}

double * bpm_fft_impl_kissfft::get_input_buffer()
{
	return m_input_buffer.get_ptr();
}

double * bpm_fft_impl_kissfft::get_output_buffer()
{
	return m_output_buffer.get_ptr();
}

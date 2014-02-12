#include "stdafx.h"
#include "bpm_fft_impl_fftw.h"

critical_section2 bpm_fft_impl_fftw::g_plan_mutex;

bpm_fft_impl_fftw::bpm_fft_impl_fftw():
	m_plan(nullptr)
{
}

bpm_fft_impl_fftw::~bpm_fft_impl_fftw()
{
	if (m_plan != nullptr)
	{
		c_insync2 lock(g_plan_mutex);

		fftw_destroy_plan(m_plan);
	}
}

void bpm_fft_impl_fftw::create_plan(int p_size)
{
	c_insync2 lock(g_plan_mutex);

	if (m_plan != nullptr)
	{
		fftw_destroy_plan(m_plan);
		m_plan = nullptr;
	}

	m_input_buffer = fftw_alloc_real(p_size);
	m_output_buffer = fftw_alloc_real(p_size);
	m_plan = fftw_plan_r2r_1d(p_size, m_input_buffer.get_ptr(), m_output_buffer.get_ptr(), FFTW_R2HC, FFTW_MEASURE);
}

void bpm_fft_impl_fftw::execute_plan()
{
	PFC_ASSERT(m_plan != nullptr);

	fftw_execute(m_plan);
}

void bpm_fft_impl_fftw::destroy_plan()
{
	c_insync2 lock(g_plan_mutex);

	if (m_plan != nullptr)
	{
		fftw_destroy_plan(m_plan);
		m_plan = nullptr;
		//fftw_free(m_in);
		//fftw_free(m_out);
	}
}

double * bpm_fft_impl_fftw::get_input_buffer()
{
	return m_input_buffer.get_ptr();
}

double * bpm_fft_impl_fftw::get_output_buffer()
{
	return m_output_buffer.get_ptr();
}

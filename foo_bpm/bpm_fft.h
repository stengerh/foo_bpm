#ifndef __BPM_FFT_H__
#define __BPM_FFT_H__

#include "pfc/pfc.h"

class NOVTABLE bpm_fft : public pfc::refcounted_object_root
{
public:
	typedef pfc::refcounted_object_ptr_t<bpm_fft> ptr;

	virtual ~bpm_fft() {}

	virtual void create_plan(int p_size) = 0;
	virtual void execute_plan() = 0;
	virtual void destroy_plan() = 0;
	virtual double * get_input_buffer() = 0;
	virtual double * get_output_buffer() = 0;

	static bpm_fft::ptr g_create();
};

#endif // __BPM_FFT_H__

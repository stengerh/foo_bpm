#ifndef __FORMAT_BPM_H__
#define __FORMAT_BPM_H__

#include "pfc/pfc.h"

#include "preferences.h"

class format_bpm
{
public:
	explicit format_bpm(double p_bpm);
	format_bpm(double p_bpm, bpm_precision_enum p_precision);

	inline const char * get_ptr() const {return m_formatter.get_ptr();}
	inline const char * toString() const {return m_formatter.get_ptr();}
	inline operator const char*() const {return m_formatter.get_ptr();};

private:
	void format(double p_bpm, int p_precision);

	pfc::string_formatter m_formatter;
};

#endif // __FORMAT_BPM_H__

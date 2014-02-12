#include "stdafx.h"
#include "format_bpm.h"

#include "globals.h"

format_bpm::format_bpm(double p_bpm)
{
	format(p_bpm, bpm_config_bpm_precision);
}

format_bpm::format_bpm(double p_bpm, bpm_precision_enum p_precision)
{
	format(p_bpm, p_precision);
}

void format_bpm::format(double p_bpm, int p_precision)
{
	switch (p_precision)
	{
	case BPM_PRECISION_1DP:
		m_formatter << pfc::format_float(p_bpm, 0, 1);
		break;
	case BPM_PRECISION_2DP:
		m_formatter << pfc::format_float(p_bpm, 0, 2);
		break;
	case BPM_PRECISION_1:
	default:
		m_formatter << pfc::format_float(p_bpm, 0, 0);
		break;
	}
}

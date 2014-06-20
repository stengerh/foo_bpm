#include "stdafx.h"

#include "file_info_filter_scale_bpm.h"

#include "format_bpm.h"

file_info_filter_scale_bpm::file_info_filter_scale_bpm(const char * p_bpm_tag, double p_scale)
	: m_bpm_tag(p_bpm_tag)
	, m_scale(p_scale)
{
}

bool file_info_filter_scale_bpm::apply_filter(metadb_handle_ptr p_track, t_filestats p_stats, file_info & p_info)
{
	const char * str = p_info.meta_get(m_bpm_tag, 0);

	float bpm = 0.0f;
	if ((str != NULL) && (sscanf_s(str, "%f", &bpm) == 1))
	{
		bpm = static_cast<float>(bpm * m_scale);

		p_info.meta_set(m_bpm_tag, format_bpm(bpm));

		return true;
	}
	else
	{
		return false;
	}
}

#ifndef __FILE_INFO_FILTER_SCALE_BPM_H__
#define __FILE_INFO_FILTER_SCALE_BPM_H__

#include "foobar2000/SDK/foobar2000.h"

class file_info_filter_scale_bpm : public file_info_filter
{
public:
	file_info_filter_scale_bpm(const char * p_bpm_tag, double p_scale);
	bool apply_filter(metadb_handle_ptr p_track, t_filestats p_stats, file_info & p_info);

private:
	pfc::string8 m_bpm_tag;
	double m_scale;
};

#endif // __FILE_INFO_FILTER_SCALE_BPM_H__

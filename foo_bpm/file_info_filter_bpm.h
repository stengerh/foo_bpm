#ifndef __FILE_INFO_FILTER_BPM_H__
#define __FILE_INFO_FILTER_BPM_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"

class file_info_filter_bpm : public file_info_filter
{
public:
	file_info_filter_bpm(const metadb_handle_list & p_tracks, const char * p_bpm_tag, const std::vector<double> & p_bpm_results);
	bool apply_filter(metadb_handle_ptr p_track, t_filestats p_stats, file_info & p_info);

private:
	metadb_handle_list m_tracks;
	std::vector<double> m_bpm_results;
	pfc::string8 m_bpm_tag;
};

#endif // __FILE_INFO_FILTER_BPM_H__

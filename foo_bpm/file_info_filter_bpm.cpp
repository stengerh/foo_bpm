#include "stdafx.h"

#include "file_info_filter_bpm.h"

#include "format_bpm.h"

file_info_filter_bpm::file_info_filter_bpm(const metadb_handle_list & p_tracks, const char * p_bpm_tag, const std::vector<double> & p_bpm_results)
	: m_bpm_tag(p_bpm_tag)
{
	pfc::dynamic_assert(p_tracks.get_count() == p_bpm_results.size());
	pfc::array_t<t_size> order;
	order.set_size(p_tracks.get_count());
	order_helper::g_fill(order.get_ptr(), order.get_size());
	p_tracks.sort_get_permutation_t(pfc::compare_t<metadb_handle_ptr, metadb_handle_ptr>, order.get_ptr());
	m_tracks.set_count(order.get_size());
	m_bpm_results.resize(order.get_size());

	for(t_size n = 0; n < order.get_size(); n++)
	{
		m_tracks[n] = p_tracks[order[n]];
		m_bpm_results[n] = p_bpm_results[order[n]];
	}
}

file_info_filter_bpm::file_info_filter_bpm(metadb_handle_ptr p_track, const char * p_bpm_tag, double p_bpm_result)
	: m_bpm_tag(p_bpm_tag)
{
	m_tracks.add_item(p_track);
	m_bpm_results.push_back(p_bpm_result);
}

bool file_info_filter_bpm::apply_filter(metadb_handle_ptr p_track, t_filestats p_stats, file_info & p_info)
{
	t_size index;
	if (m_tracks.bsearch_t(pfc::compare_t<metadb_handle_ptr, metadb_handle_ptr>, p_track, index))
	{
		format_bpm bpm_value(m_bpm_results[index]);
		p_info.meta_set(m_bpm_tag, bpm_value);
		return true;
	}
	else
	{
		return false;
	}
}

#include "stdafx.h"
#include "bpm_contextmenu_item.h"
#include "guid.h"
#include "foo_bpm.h"
#include "bpm_auto_analysis_thread.h"
#include "format_bpm.h"

static contextmenu_group_popup_factory g_bpm_context_group(guid_bpm_context_group, contextmenu_groups::root, "BPM Analyser", 0);


GUID bpm_contextmenu_item::get_parent()
{
	return guid_bpm_context_group;
}

unsigned bpm_contextmenu_item::get_num_items()
{
	return mnuTotal;
}

void bpm_contextmenu_item::get_item_name(unsigned p_index, pfc::string_base & p_out)
{
	switch (p_index)
	{
	    case mnuAutoAnalysis:
		    p_out = "Automatically analyse BPMs";
			break;
		case mnuManualAnalysis:
			p_out = "Manually tap BPM for current track";
			break;
		case mnuDoubleBPM:
			p_out = "Double selected BPMs";
			break;
		case mnuHalveBPM:
			p_out = "Halve selected BPMs";
			break;
		default:
			break;
	}
}

void bpm_contextmenu_item::context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller)
{
	switch (p_index)
	{
	    case mnuAutoAnalysis:
			run_auto_analysis(p_data);
			break;
		case mnuManualAnalysis:
			run_manual_analysis(p_data);
			break;
		case mnuDoubleBPM:
			run_scale_bpm(p_data, 2.0);
			break;
		case mnuHalveBPM:
			run_scale_bpm(p_data, 0.5);
			break;
		default:
			break;
	}
}

GUID bpm_contextmenu_item::get_item_guid(unsigned p_index)
{
	switch (p_index)
	{
	    case mnuAutoAnalysis:
			return guid_auto_bpm;
			break;
		case mnuManualAnalysis:
			return guid_manual_bpm;
			break;
		case mnuDoubleBPM:
			return guid_double_bpm;
			break;
		case mnuHalveBPM:
			return guid_halve_bpm;
			break;
		default:
			break;
	}

	return pfc::guid_null;
}

bool bpm_contextmenu_item::get_item_description(unsigned p_index, pfc::string_base & p_out)
{
	switch (p_index)
	{
	    case mnuAutoAnalysis:
	        p_out = "Perform an automatic BPM analysis of the selected tracks.";
			break;
		case mnuManualAnalysis:
			p_out = "Manually tap the BPM of the selected track.";
			break;
		case mnuDoubleBPM:
			p_out = "Double the BPM of the selected tracks.";
			break;
		case mnuHalveBPM:
			p_out = "Halve the BPM of the selected tracks.";
			break;
		default:
			return false;
			break;
	}

	return true;
}

void bpm_contextmenu_item::run_auto_analysis(metadb_handle_list_cref p_data)
{
	service_ptr_t<bpm_auto_analysis_thread> thread = new service_impl_t<bpm_auto_analysis_thread>(p_data);
	thread->start();
}

void bpm_contextmenu_item::run_manual_analysis(metadb_handle_list_cref p_data)
{
	bpm_manual_dialog* dlg = new bpm_manual_dialog();
	dlg->Create(core_api::get_main_window(), NULL);
	dlg->ShowWindow(SW_SHOWNORMAL);
}

void bpm_contextmenu_item::run_scale_bpm(metadb_handle_list_cref p_data, double p_scale)
{
	const pfc::string8 bpm_tag = bpm_config_bpm_tag;

	metadb_handle_list tracks;
	pfc::list_t<file_info_impl> infos;

	tracks.prealloc(p_data.get_size());
	infos.prealloc(p_data.get_size());

	// For each item in the playlist selection
	for (t_size index = 0; index < p_data.get_size(); index++)
	{
		metadb_handle_ptr track = p_data[index];
		file_info_impl info;

		if (track->get_info(info) && info.meta_exists(bpm_tag))
		{
			tracks.add_item(track);
			infos.add_item(info);
		}
	}

	for (t_size index = 0; index < infos.get_size(); index++)
	{
		const char * str = infos[index].meta_get(bpm_tag, 0);

		float bpm = 0.0f;
		if (sscanf_s(str, "%f", &bpm) == 1)
		{
			bpm = static_cast<float>(bpm * p_scale);

			infos[index].meta_set(bpm_tag, format_bpm(bpm));
		}
	}

	static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(tracks,
		pfc::ptr_list_const_array_t<const file_info, file_info_impl *>(infos.get_ptr(), infos.get_size()),
		core_api::get_main_window(),
		/* p_op_flags */ 0,
		/* p_notify */ NULL);
}

static contextmenu_item_factory_t<bpm_contextmenu_item> contextmenu_factory;

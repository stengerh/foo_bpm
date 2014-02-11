#include "stdafx.h"
#include "context_menu.h"
#include "bpm_auto_analysis_thread.h"

static contextmenu_group_popup_factory g_bpm_context_group(guid_bpm_context_group, contextmenu_groups::root, "BPM Analysis", 0);


GUID contextmenu_item_simple_bpm::get_parent()
{
	return guid_bpm_context_group;
}

unsigned contextmenu_item_simple_bpm::get_num_items()
{
	return mnuTotal;
}

void contextmenu_item_simple_bpm::get_item_name(unsigned p_index, pfc::string_base & p_out)
{
	switch (p_index)
	{
	    case mnuAutoAnalysis:
		    p_out = "Automatically analyse BPMs...";
			break;
		case mnuManualAnalysis:
			p_out = "Manually tap BPM for current track...";
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

void contextmenu_item_simple_bpm::context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller)
{
	bpm_manual_dialog* m_bpm_manual_dialog;
	
	int p_data_size = p_data.get_size();
	file_info_impl f_info;
	float bpm = 0;
	const char* str = "";
	char bpm_str[256];
	pfc::list_t<metadb_handle_ptr> p_data_local;
	pfc::list_t<file_info_impl> p_info;

	switch (p_index)
	{
	    case mnuAutoAnalysis:
			{
				service_ptr_t<bpm_auto_analysis_thread> thread = new service_impl_t<bpm_auto_analysis_thread>(p_data);
				thread->start();
			}
			break;
		case mnuManualAnalysis:
			m_bpm_manual_dialog = new bpm_manual_dialog();
			m_bpm_manual_dialog->Create(core_api::get_main_window(), NULL);
			m_bpm_manual_dialog->ShowWindow(SW_SHOWNORMAL);
			break;
		case mnuDoubleBPM:
		case mnuHalveBPM:
			p_data_local.set_size(p_data_size);
			p_info.set_size(p_data_size);
			for (int i = 0; i < p_data_size; i++)
			{
				p_data_local[i] = p_data[i];
			}

			// For each item in the playlist selection
			for (int item = 0; item < p_data_size; item++)
			{
				if (!p_data_local[item]->get_info(p_info[item]))
				{
					p_data_local.remove_by_idx(item);
					p_info.remove_by_idx(item);
					p_data_size--;
					item--;
					continue;
				}
				
				// Are any of the tracks missing the BPM tag?
				if (p_info[item].meta_get(bpm_config_bpm_tag.get_ptr(), 0) == 0)
				{
					p_data_local.remove_by_idx(item);
					p_info.remove_by_idx(item);
					p_data_size--;
					item--;
					continue;
				}
			}


			for (int i = 0; i < p_data_size; i++)
			{
				str = p_info[i].meta_get(bpm_config_bpm_tag.get_ptr(), 0);

				sscanf_s(str, "%f", &bpm);

				if (p_index == mnuDoubleBPM)
				{
					bpm = bpm * 2;
				}
				else if (p_index == mnuHalveBPM)
				{
					bpm = bpm / 2;
				}

				switch (bpm_config_bpm_precision)
				{
					case BPM_PRECISION_1:
						sprintf_s(bpm_str, "%d", (int)bpm);
						break;
					case BPM_PRECISION_1DP:
						sprintf_s(bpm_str, "%0.1f", bpm);
						break;
					case BPM_PRECISION_2DP:
						sprintf_s(bpm_str, "%0.2f", bpm);
						break;
				}

				p_info[i].meta_set(bpm_config_bpm_tag.get_ptr(), bpm_str);
			}

			static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(p_data_local, pfc::ptr_list_const_array_t<const file_info,
																	   file_info_impl *>(p_info.get_ptr(), p_data_size),
																	   core_api::get_main_window(), NULL, NULL);
			break;
		default:
			break;
	}
}

GUID contextmenu_item_simple_bpm::get_item_guid(unsigned p_index)
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

bool contextmenu_item_simple_bpm::get_item_description(unsigned p_index, pfc::string_base & p_out)
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

static contextmenu_item_factory_t<contextmenu_item_simple_bpm> contextmenu_factory;
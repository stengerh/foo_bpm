#include "stdafx.h"
#include "bpm_auto_analysis_thread.h"
#include "bpm_auto_analysis.h"
#include "preferences.h"
#include "dialog.h"

/***** Threading *****/

bpm_auto_analysis_thread::bpm_auto_analysis_thread(metadb_handle_list_cref playlist_items)
{
	p_data_size = playlist_items.get_size();
	p_data.set_size(p_data_size);
	p_info.set_size(p_data_size);
	for (int i = 0; i < p_data_size; i++)
	{
		p_data[i] = playlist_items[i];
	}
}

void bpm_auto_analysis_thread::start()
{
	bool bpm_missing = false;
	bool rescan = false;

	// For each item in the playlist selection
	for (int item = 0; item < p_data_size; item++)
    {
		if (!p_data[item]->get_info(p_info[item]))
		{
			p_data.remove_by_idx(item);
			p_info.remove_by_idx(item);
			p_data_size--;
			item--;
			continue;
		}
		
		// Are any of the tracks missing the BPM?
		if (!bpm_missing && p_info[item].meta_get(bpm_config_bpm_tag.get_ptr(), 0) == 0)
		{
			bpm_missing = true;
		}
	}

	if (bpm_missing)
	{
		// For each item in the playlist selection
		for (int item = 0; item < p_data_size; item++)
		{
			// Remove any items that already have a BPM set. We only want to scan BPM-less tracks
			if (p_info[item].meta_get(bpm_config_bpm_tag.get_ptr(), 0) != 0)
			{
				p_data.remove_by_idx(item);
				p_info.remove_by_idx(item);
				p_data_size--;
				item--;
			}
		}
	}
	else
	{
		int response = MessageBox(core_api::get_main_window(),
					_T("All tracks you have selected already have BPM info. Would you like to scan them anyway?"),
					_T("Information"),
					MB_YESNO);

		if (response == IDYES)
		{
			rescan = true;
		}
	}

	if (rescan || bpm_missing)
	{
		threaded_process::g_run_modeless( 
			this,
			threaded_process::flag_show_abort | 
			threaded_process::flag_show_delayed |
			threaded_process::flag_show_minimize |
			threaded_process::flag_show_progress_dual |
			threaded_process::flag_show_item,
			core_api::get_main_window(),
			"Analysing BPMs..."
			);
	}
}

void bpm_auto_analysis_thread::run(threaded_process_status & p_status, abort_callback & p_abort)
{
	bpm_results.resize(0);

	p_status.set_progress_secondary(0, p_data_size);

	// For each item in the playlist selection
	for (int item = 0; item < p_data_size; item++)
    {
		// Skip the file if it doesn't exist
		if (!filesystem::g_exists(p_data[item]->get_path(), p_abort))
		{
			p_data.remove_by_idx(item);
			p_info.remove_by_idx(item);
			p_data_size--;
			item--;
		}
		else
		{
			p_status.set_item_path(p_data[item]->get_location().get_path(), ~0);
			bpm_auto_analysis bpm(p_data[item]);
			bpm_results.push_back(bpm.run_safe(p_status, p_abort));

			p_status.set_progress_secondary(item+1, p_data_size);
		}

		if (p_abort.is_aborting()) break;
	}
}

void bpm_auto_analysis_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (!p_was_aborted && core_api::assert_main_thread())
	{
		bpm_result_dialog* m_result_dialog = new bpm_result_dialog();
		m_result_dialog->p_data = p_data;
		m_result_dialog->p_info = p_info;
		m_result_dialog->p_data_size = p_data_size;
		m_result_dialog->bpm_results = bpm_results;

		m_result_dialog->Create(core_api::get_main_window(), NULL);
		m_result_dialog->ShowWindow(SW_SHOWNORMAL);
	}
}

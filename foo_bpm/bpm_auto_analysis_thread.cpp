#include "stdafx.h"
#include "bpm_auto_analysis_thread.h"
#include "bpm_auto_analysis.h"
#include "preferences.h"
#include "bpm_result_dialog.h"

/***** Threading *****/

bpm_auto_analysis_thread::bpm_auto_analysis_thread(metadb_handle_list_cref p_tracks)
{
	m_tracks.add_items(p_tracks);
	m_infos.set_size(m_tracks.get_count());
}

void bpm_auto_analysis_thread::start()
{
	bool bpm_missing = false;
	bool rescan = false;

	{
		bit_array_bittable mask(m_tracks.get_size());

		// For each item in the playlist selection
		for (t_size index = 0; index < m_tracks.get_size(); index++)
		{
			if (m_tracks[index]->get_info(m_infos[index]))
			{
				// Are any of the tracks missing the BPM?
				if (!m_infos[index].meta_exists(bpm_config_bpm_tag))
				{
					bpm_missing = true;
				}
			}
			else
			{
				mask.set(index, true);
			}
		}

		m_tracks.remove_mask(mask);
		m_infos.remove_mask(mask);
	}

	if (bpm_missing)
	{
		bit_array_bittable mask(m_tracks.get_count());

		// For each item in the playlist selection
		for (t_size index = 0; index < m_tracks.get_size(); index++)
		{
			// Remove any items that already have a BPM set. We only want to scan BPM-less tracks
			mask.set(index, m_infos[index].meta_exists(bpm_config_bpm_tag));
		}

		m_tracks.remove_mask(mask);
		m_infos.remove_mask(mask);
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
	m_bpm_results.resize(0);

	p_status.set_progress(0, m_tracks.get_size());

	// For each item in the playlist selection
	for (t_size index = 0; index < m_tracks.get_size(); index++)
    {
		// Skip the file if it doesn't exist
		if (!filesystem::g_exists(m_tracks[index]->get_path(), p_abort))
		{
			m_tracks.remove_by_idx(index);
			m_infos.remove_by_idx(index);
			index--;
		}
		else
		{
			p_status.set_item_path(m_tracks[index]->get_location().get_path());

			bpm_auto_analysis bpm(m_tracks[index]);
			double bpm_result = bpm.run_safe(p_status, p_abort);
			m_bpm_results.push_back(bpm_result);

			p_status.set_progress(index+1, m_tracks.get_size());
		}

		if (p_abort.is_aborting()) break;
	}
}

void bpm_auto_analysis_thread::on_done(HWND p_wnd, bool p_was_aborted)
{
	if (!p_was_aborted && core_api::assert_main_thread())
	{
		bpm_result_dialog* m_result_dialog = new bpm_result_dialog(m_tracks, m_infos, m_bpm_results);

		m_result_dialog->Create(core_api::get_main_window(), NULL);
		m_result_dialog->ShowWindow(SW_SHOWNORMAL);
	}
}

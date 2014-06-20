#include "stdafx.h"

#include <string>

#include "bpm_manual_dialog.h"
#include "globals.h"
#include "preferences.h"
#include "format_bpm.h"
#include "file_info_filter_bpm.h"

using std::string;

bpm_manual_dialog::bpm_manual_dialog():
	m_bpm(0.0)
{
	LARGE_INTEGER res;

	QueryPerformanceFrequency(&res);
	timer_resolution = res.QuadPart;
	time_to_reset_average = bpm_config_seconds_to_reset_average * timer_resolution;
}

LRESULT bpm_manual_dialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	pfc::string_formatter bpm_tag_label;
	bpm_tag_label << "BPM will be written to %" << bpm_config_bpm_tag.get_ptr() << "% tag.";
	uSetDlgItemText(m_hWnd, ID_MANUAL_BPM_TAG, bpm_tag_label);

	ResetBPM();

	return 0;
}

LRESULT bpm_manual_dialog::OnUpdateFileClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	metadb_handle_ptr track;

	if (core_api::assert_main_thread() && static_api_ptr_t<playback_control>()->get_now_playing(track))
	{
		static_api_ptr_t<metadb_io_v2>()->update_info_async(
			pfc::list_single_ref_t<metadb_handle_ptr>(track),
			new service_impl_t<file_info_filter_bpm>(track, bpm_config_bpm_tag, m_bpm),
			core_api::get_main_window(),
			0, NULL);
	}

	return 0;
}

LRESULT bpm_manual_dialog::OnTapClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	if (!tap_times.empty() &&
		((tap_times[tap_times.size() - 1] - time.QuadPart) > time_to_reset_average))
	{
		ResetBPM();
	}
	else
	{
		if (tap_times.size() >= (size_t) bpm_config_taps_to_average)
		{
			tap_times.erase(tap_times.begin());
		}

		tap_times.push_back(time.QuadPart);

		if (tap_times.size() > 1)
		{
			double avg_time_diff = 0.0;

			for (unsigned index = 1; index < tap_times.size(); index++)
			{
				avg_time_diff += (tap_times[index] - tap_times[index-1]);
			}

			avg_time_diff = avg_time_diff / ((double) tap_times.size() - 1.0);
			double bpm = 60.0 / (avg_time_diff / (double) timer_resolution);

			SetBPM(bpm);
		}
	}

	return 0;
}

LRESULT bpm_manual_dialog::OnResetClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ResetBPM();
	return 0;
}

LRESULT bpm_manual_dialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DestroyWindow();
	return 0;
}

void bpm_manual_dialog::OnClose()
{
	DestroyWindow();
}

void bpm_manual_dialog::PostNcDestroy()
{
	delete this;
}

bool bpm_manual_dialog::pretranslate_message(MSG *p_msg)
{
	if (m_hWnd != NULL)
	{
		if (IsDialogMessage(p_msg))
		{
			return true;
		}
	}

	return false;
}

void bpm_manual_dialog::ResetBPM()
{
	tap_times.clear();
	tap_times.reserve(bpm_config_taps_to_average);
	SetBPM(0.0);
}

void bpm_manual_dialog::SetBPM(double bpm)
{
	m_bpm = bpm;
	uSetDlgItemText(m_hWnd, ID_BPM_MANUAL_BPM, format_bpm(m_bpm));
}

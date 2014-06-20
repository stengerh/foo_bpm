#include "stdafx.h"

#include <string>

#include "bpm_result_dialog.h"
#include "preferences.h"
#include "format_bpm.h"
#include "file_info_filter_bpm.h"

using std::string;

bpm_result_dialog::bpm_result_dialog(metadb_handle_list_cref p_tracks, const pfc::list_t<file_info_impl> &p_infos, const std::vector<double> &p_bpm_results):
	m_tracks(p_tracks),
	m_infos(p_infos),
	m_bpm_results(p_bpm_results)
{
}

LRESULT bpm_result_dialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	if (bpm_config_auto_write_tag)
	{
		static_api_ptr_t<metadb_io_v2>()->update_info_async(
			m_tracks,
			new service_impl_t<file_info_filter_bpm>(m_tracks, bpm_config_bpm_tag, m_bpm_results),
			core_api::get_main_window(),
			metadb_io_v2::op_flag_background | metadb_io_v2::op_flag_delay_ui,
			NULL);

		DestroyWindow();
	}
	else
	{
		CListViewCtrl result_list = GetDlgItem(ID_BPM_RESULT_LIST);

		listview_helper::insert_column(result_list, 0, "Title", 270);
		// TODO: Remember status of scan result (ie. success, ambiguous, double, half)
	//	 listview_helper::insert_column(result_list, 1, "Status", 60);
		listview_helper::insert_column(result_list, 1, "BPM", 50);
		// TODO: Allow selection of an alternate BPM
	//	 listview_helper::insert_column(result_list, 3, "BPM (Alt)", 50);

		result_list.SetExtendedListViewStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);// | LVS_EX_CHECKBOXES);

		string title_column;

		for (t_size index = 0; index < m_infos.get_size(); index++)
		{
			if (m_infos[index].meta_exists("TITLE"))
				title_column = m_infos[index].meta_get("TITLE", 0);
			else
				title_column = pfc::string_filename(m_tracks[index]->get_path());

			listview_helper::insert_item(result_list, index, title_column.c_str(), 0);

			format_bpm bpm_value(m_bpm_results[index]);

			listview_helper::set_item_text(result_list, index, 1, bpm_value);
		}

		pfc::string_formatter bpm_tag_label;
		bpm_tag_label << "BPM will be written to %" << bpm_config_bpm_tag.get_ptr() << "% tag.";
		uSetDlgItemText(m_hWnd, ID_RESULT_BPM_TAG, bpm_tag_label);

		EnableScaleBPMButtons();
	}

	return 0;
}

LRESULT bpm_result_dialog::OnOK(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	static_api_ptr_t<metadb_io_v2>()->update_info_async(
		m_tracks,
		new service_impl_t<file_info_filter_bpm>(m_tracks, bpm_config_bpm_tag, m_bpm_results),
		core_api::get_main_window(),
		metadb_io_v2::op_flag_background | metadb_io_v2::op_flag_delay_ui,
		NULL);

	DestroyWindow();
	return 0;
}

LRESULT bpm_result_dialog::OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	DestroyWindow();
	return 0;
}

LRESULT bpm_result_dialog::OnDoubleBPMClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ScaleSelectionBPM(2.0);
	return 0;
}

LRESULT bpm_result_dialog::OnHalveBPMClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	ScaleSelectionBPM(0.5);
	return 0;
}

LRESULT bpm_result_dialog::OnItemChanged(LPNMHDR pnmh)
{
	EnableScaleBPMButtons();

	return 0;
}

void bpm_result_dialog::OnClose()
{
	DestroyWindow();
}

void bpm_result_dialog::PostNcDestroy()
{
	delete this;
}

bool bpm_result_dialog::pretranslate_message(MSG *p_msg)
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

void bpm_result_dialog::EnableScaleBPMButtons()
{
	CListViewCtrl listView(GetDlgItem(ID_BPM_RESULT_LIST));

	UINT selected = listView.GetSelectedCount();

	GetDlgItem(ID_DOUBLE_BPM_BUTTON).EnableWindow(selected > 0);
	GetDlgItem(ID_HALVE_BPM_BUTTON).EnableWindow(selected > 0);
}

void bpm_result_dialog::ScaleSelectionBPM(double p_factor)
{
	CWindow result_list = GetDlgItem(ID_BPM_RESULT_LIST);

	int listview_index = -1;
	while ((listview_index = ListView_GetNextItem(result_list, listview_index, LVIS_SELECTED)) != -1)
	{
		m_bpm_results[listview_index] = m_bpm_results[listview_index] * p_factor;

		format_bpm bpm_value(m_bpm_results[listview_index]);

		m_infos[listview_index].meta_set(bpm_config_bpm_tag, bpm_value);
		listview_helper::set_item_text(result_list, listview_index, 1, bpm_value);
	}
}

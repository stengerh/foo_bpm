#include "stdafx.h"

#include <string>

#include "bpm_result_dialog.h"
#include "preferences.h"

using std::string;

LRESULT bpm_result_dialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	CWindow result_list = GetDlgItem(ID_BPM_RESULT_LIST);
	char bpm_str[16];

	listview_helper::insert_column(result_list, 0, "Title", 270);
	// TODO: Remember status of scan result (ie. success, ambiguous, double, half)
//	 listview_helper::insert_column(result_list, 1, "Status", 60);
	listview_helper::insert_column(result_list, 1, "BPM", 50);
	// TODO: Allow selection of an alternate BPM
//	 listview_helper::insert_column(result_list, 3, "BPM (Alt)", 50);

	ListView_SetExtendedListViewStyle(result_list, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);// | LVS_EX_CHECKBOXES);

	string title_column;

	for (int i = 0; i < p_data_size; i++)
	{
		if (p_info[i].meta_get("TITLE", 0) != 0)
			title_column = p_info[i].meta_get("TITLE", 0);
		else
			title_column = pfc::string_filename(p_data[i]->get_path());

		listview_helper::insert_item(result_list, i, title_column.c_str(), 0);
//		listview_helper::set_item_text(result_list, i, 1, "Success");

		switch (bpm_config_bpm_precision)
		{
			case BPM_PRECISION_1:
				sprintf_s(bpm_str, "%d", (int)bpm_results[i]);
				break;
			case BPM_PRECISION_1DP:
				sprintf_s(bpm_str, "%0.1f", bpm_results[i]);
				break;
			case BPM_PRECISION_2DP:
				sprintf_s(bpm_str, "%0.2f", bpm_results[i]);
				break;
		}
		p_info[i].meta_set(bpm_config_bpm_tag.get_ptr(), bpm_str);
		listview_helper::set_item_text(result_list, i, 1, p_info[i].meta_get(bpm_config_bpm_tag.get_ptr(), 0));
	}

	char a_string[256];
	wchar_t w_string[256];
	sprintf_s(a_string, "BPMs will be written to %%%s%% tag.", bpm_config_bpm_tag.get_ptr());
	pfc::stringcvt::convert_ansi_to_wide(w_string, 256, a_string, bpm_config_bpm_tag.length() + 31);
	SetDlgItemText(ID_RESULT_BPM_TAG, w_string);

	if (bpm_config_auto_write_tag)
	{
		static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(p_data, pfc::ptr_list_const_array_t<const file_info,
																   file_info_impl *>(p_info.get_ptr(), p_data_size),
																   core_api::get_main_window(), NULL, NULL);

		DestroyWindow();
	}

	return 0;
}

LRESULT bpm_result_dialog::OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CWindow result_list = GetDlgItem(ID_BPM_RESULT_LIST);
	int listview_index = -1;
	double temp_bpm = 0;
	char bpm_str[16];

	switch (nID)
	{
		case ID_UPDATE_TAGS_BUTTON:
			static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(p_data, pfc::ptr_list_const_array_t<const file_info,
																	   file_info_impl *>(p_info.get_ptr(), p_data_size),
																	   core_api::get_main_window(), NULL, NULL);
			DestroyWindow();
			break;
		case ID_CANCEL_BUTTON:
			DestroyWindow();
			break;
		case ID_DOUBLE_BPM_BUTTON:
		case ID_HALVE_BPM_BUTTON:
			// TODO: Add key modifier for select all (ctrl-A)
			while ((listview_index = ListView_GetNextItem(result_list, listview_index, LVIS_SELECTED)) != -1)
			{
				if (nID == ID_DOUBLE_BPM_BUTTON)
				{
					bpm_results[listview_index] = bpm_results[listview_index] * 2;
				}
				else if (nID == ID_HALVE_BPM_BUTTON)
				{
					bpm_results[listview_index] = bpm_results[listview_index] / 2;
				}

				switch (bpm_config_bpm_precision)
				{
					case BPM_PRECISION_1:
						sprintf_s(bpm_str, "%d", (int)bpm_results[listview_index]);
						break;
					case BPM_PRECISION_1DP:
						sprintf_s(bpm_str, "%0.1f", bpm_results[listview_index]);
						break;
					case BPM_PRECISION_2DP:
						sprintf_s(bpm_str, "%0.2f", bpm_results[listview_index]);
						break;
				}
				p_info[listview_index].meta_set(bpm_config_bpm_tag.get_ptr(), bpm_str);
				listview_helper::set_item_text(result_list, listview_index, 1, p_info[listview_index].meta_get(bpm_config_bpm_tag.get_ptr(), 0));
			}
			break;
		default:
			break;
	}

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

#include "dialog.h"

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






bpm_manual_dialog::bpm_manual_dialog()
{
	LARGE_INTEGER res;

	tap_times.reserve(bpm_config_taps_to_average);
	QueryPerformanceFrequency(&res);
	timer_resolution = res.QuadPart;
	time_to_reset_average = bpm_config_seconds_to_reset_average * timer_resolution;
	manual_bpm = 0;
}

LRESULT bpm_manual_dialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	char a_string[256];
	wchar_t w_string[256];
	sprintf_s(a_string, "BPM will be written to %%%s%% tag.", bpm_config_bpm_tag.get_ptr());
	pfc::stringcvt::convert_ansi_to_wide(w_string, 256, a_string, bpm_config_bpm_tag.length() + 31);
	SetDlgItemText(ID_MANUAL_BPM_TAG, w_string);

	return 0;
}

LRESULT bpm_manual_dialog::OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	char a_string[256];
	wchar_t w_string[256];
	int a_length = 0;
	LARGE_INTEGER time;
	metadb_handle_ptr item;
	pfc::list_t<metadb_handle_ptr> p_item;
	pfc::list_t<file_info_impl> p_info;

	switch (nID)
	{
		case ID_BPM_MANUAL_TAP_BUTTON:
			if ((int)tap_times.size() >= bpm_config_taps_to_average)
			{
				tap_times.erase(tap_times.begin());
			}
			QueryPerformanceCounter(&time);
			tap_times.push_back(time.QuadPart);

			manual_bpm = 0;

			if ((tap_times.size() >= 2) &&
				(tap_times[tap_times.size()-1] - tap_times[tap_times.size()-2]) > time_to_reset_average)
			{
				tap_times.clear();
				tap_times.reserve(bpm_config_taps_to_average);
			}
			else if (tap_times.size() > 1)
			{
				for (unsigned index = 1; index < tap_times.size(); index++)
				{
					manual_bpm += (tap_times[index] - tap_times[index-1]);
				}

				manual_bpm = manual_bpm / ((double)tap_times.size() - 1);
				manual_bpm = 60.0 / (manual_bpm / timer_resolution);
			}

			switch (bpm_config_bpm_precision)
			{
				case BPM_PRECISION_1:
					a_length = sprintf_s(a_string, "%d", (int)manual_bpm);
					break;
				case BPM_PRECISION_1DP:
					a_length = sprintf_s(a_string, "%0.1f", manual_bpm);
					break;
				case BPM_PRECISION_2DP:
					a_length = sprintf_s(a_string, "%0.2f", manual_bpm);
					break;
			}
			pfc::stringcvt::convert_ansi_to_wide(w_string, 256, a_string, a_length);
			SetDlgItemText(ID_BPM_MANUAL_BPM, w_string);

			break;
		case ID_BPM_MANUAL_RESET_BUTTON:
			tap_times.clear();
			tap_times.reserve(bpm_config_taps_to_average);
			manual_bpm = 0;
			sprintf_s(a_string, "0");
			SetDlgItemText(ID_BPM_MANUAL_BPM, _T("0"));
			break;
		case ID_BPM_MANUAL_UPDATE_TAG_BUTTON:
			if (core_api::assert_main_thread() && static_api_ptr_t<playback_control>()->get_now_playing(item))
			{
				switch (bpm_config_bpm_precision)
				{
					case BPM_PRECISION_1:
						a_length = sprintf_s(a_string, "%d", (int)manual_bpm);
						break;
					case BPM_PRECISION_1DP:
						a_length = sprintf_s(a_string, "%0.1f", manual_bpm);
						break;
					case BPM_PRECISION_2DP:
						a_length = sprintf_s(a_string, "%0.2f", manual_bpm);
						break;
				}

				p_item.set_size(1);
				p_info.set_size(1);

				p_item[0] = item;
				p_item[0]->get_info(p_info[0]);
				p_info[0].meta_set(bpm_config_bpm_tag.get_ptr(), a_string);

				static_api_ptr_t<metadb_io_v2>()->update_info_async_simple(p_item, pfc::ptr_list_const_array_t<const file_info,
																		   file_info_impl *>(p_info.get_ptr(), 1),
																		   core_api::get_main_window(), NULL, NULL);
			}
			break;
		case ID_BPM_MANUAL_CLOSE_BUTTON:
			DestroyWindow();
			break;
		default:
			break;
	}

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
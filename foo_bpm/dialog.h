#ifndef __DIALOG_H__
#define __DIALOG_H__

#include <cstring>
#include <vector>

#include "resource.h"
#include "globals.h"
#include "preferences.h"

#include "../SDK/foobar2000.h"
#include "../ATLHelpers/ATLHelpers.h"

using namespace std;

class bpm_result_dialog : public CAxDialogImpl<bpm_result_dialog>
{
	public:
		enum { IDD = IDD_BPM_RESULT_DIALOG };

		pfc::list_t<metadb_handle_ptr> p_data;
		pfc::list_t<file_info_impl> p_info;
		int p_data_size;
		vector<double> bpm_results;

		BEGIN_MSG_MAP_EX(bpm_result_dialog)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(ID_UPDATE_TAGS_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_CANCEL_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_DOUBLE_BPM_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_HALVE_BPM_BUTTON, BN_CLICKED, OnBnClicked)
			MSG_WM_CLOSE(OnClose);
		END_MSG_MAP()
		
		LRESULT OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
		LRESULT OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
		void OnClose();
		// Override the parent method for when the dialog is destroyed so we can delete its memory
		void PostNcDestroy();
};

class bpm_manual_dialog : public CAxDialogImpl<bpm_manual_dialog>
{
	public:
		enum { IDD = IDD_BPM_MANUAL_DIALOG };

		LONGLONG time_to_reset_average;
		vector<LONGLONG> tap_times;
		double manual_bpm;
		LONGLONG timer_resolution;

		BEGIN_MSG_MAP_EX(bpm_result_dialog)
			MSG_WM_INITDIALOG(OnInitDialog)
			// TODO: Only respond to button down event so we eliminate click length error
			COMMAND_HANDLER_EX(ID_BPM_MANUAL_TAP_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_BPM_MANUAL_RESET_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_BPM_MANUAL_UPDATE_TAG_BUTTON, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_BPM_MANUAL_CLOSE_BUTTON, BN_CLICKED, OnBnClicked)
			MSG_WM_CLOSE(OnClose);
		END_MSG_MAP()

		bpm_manual_dialog();
		LRESULT OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
		LRESULT OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
		void OnClose();
		// Override the parent method for when the dialog is destroyed so we can delete its memory
		void PostNcDestroy();
};

#endif
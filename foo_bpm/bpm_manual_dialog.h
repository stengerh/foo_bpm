#ifndef __BPM_MANUAL_DIALOG_H__
#define __BPM_MANUAL_DIALOG_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

#include "resource.h"

class bpm_manual_dialog : public CAxDialogImpl<bpm_manual_dialog>
{
	public:
		enum { IDD = IDD_BPM_MANUAL_DIALOG };

		LONGLONG time_to_reset_average;
		std::vector<LONGLONG> tap_times;
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

#endif // __BPM_MANUAL_DIALOG_H__

#ifndef __BPM_MANUAL_DIALOG_H__
#define __BPM_MANUAL_DIALOG_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

#include "resource.h"

class bpm_manual_dialog : public CDialogImpl<bpm_manual_dialog>
{
public:
	enum { IDD = IDD_BPM_MANUAL_DIALOG };

	LONGLONG time_to_reset_average;
	std::vector<LONGLONG> tap_times;
	LONGLONG timer_resolution;

	BEGIN_MSG_MAP_EX(bpm_result_dialog)
		MSG_WM_INITDIALOG(OnInitDialog)
		// TODO: Only respond to button down event so we eliminate click length error
		COMMAND_HANDLER_EX(ID_BPM_MANUAL_TAP_BUTTON, BN_CLICKED, OnTapClicked)
		COMMAND_HANDLER_EX(ID_BPM_MANUAL_RESET_BUTTON, BN_CLICKED, OnResetClicked)
		COMMAND_HANDLER_EX(ID_BPM_MANUAL_UPDATE_TAG_BUTTON, BN_CLICKED, OnUpdateFileClicked)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnCancel)
		MSG_WM_CLOSE(OnClose);
	END_MSG_MAP()

	bpm_manual_dialog();

private:
	LRESULT OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	LRESULT OnTapClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnResetClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnUpdateFileClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnClose();
	// Override the parent method for when the dialog is destroyed so we can delete its memory
	void PostNcDestroy();

	void ResetBPM();
	void SetBPM(double bpm);

	double m_bpm;
};

#endif // __BPM_MANUAL_DIALOG_H__

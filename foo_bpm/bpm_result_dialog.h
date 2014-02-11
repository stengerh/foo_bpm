#ifndef __DPM_RESULT_DIALOG_H__
#define __DPM_RESULT_DIALOG_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

#include "resource.h"

class bpm_result_dialog : public CDialogImpl<bpm_result_dialog>, private message_filter_impl_base
{
public:
	enum { IDD = IDD_BPM_RESULT_DIALOG };

	BEGIN_MSG_MAP_EX(bpm_result_dialog)
		// TODO: Add key modifier for select all (ctrl-A)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_HANDLER_EX(IDOK, BN_CLICKED, OnOK)
		COMMAND_HANDLER_EX(IDCANCEL, BN_CLICKED, OnCancel)
		COMMAND_HANDLER_EX(ID_DOUBLE_BPM_BUTTON, BN_CLICKED, OnDoubleBPMClicked)
		COMMAND_HANDLER_EX(ID_HALVE_BPM_BUTTON, BN_CLICKED, OnHalveBPMClicked)
		NOTIFY_HANDLER_EX(ID_BPM_RESULT_LIST, LVN_ITEMCHANGED, OnItemChanged)
		MSG_WM_CLOSE(OnClose);
	END_MSG_MAP()

	bpm_result_dialog(metadb_handle_list_cref p_tracks, const pfc::list_t<file_info_impl> &p_infos, const std::vector<double> &p_bpm_results);
		
private:
	LRESULT OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	LRESULT OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnDoubleBPMClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnHalveBPMClicked(UINT uNotifyCode, int nID, CWindow wndCtl);

	LRESULT OnItemChanged(LPNMHDR pnmh);

	void OnClose();
	// Override the parent method for when the dialog is destroyed so we can delete its memory
	void PostNcDestroy();

	bool pretranslate_message(MSG *p_msg);

	void EnableScaleBPMButtons();
	void ScaleSelectionBPM(double p_factor);

	metadb_handle_list m_tracks;
	pfc::list_t<file_info_impl> m_infos;
	std::vector<double> m_bpm_results;
};

#endif // __DPM_RESULT_DIALOG_H__

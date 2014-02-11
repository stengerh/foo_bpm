#ifndef __DPM_RESULT_DIALOG_H__
#define __DPM_RESULT_DIALOG_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

#include "resource.h"

class bpm_result_dialog : public CAxDialogImpl<bpm_result_dialog>
{
	public:
		enum { IDD = IDD_BPM_RESULT_DIALOG };

		pfc::list_t<metadb_handle_ptr> p_data;
		pfc::list_t<file_info_impl> p_info;
		int p_data_size;
		std::vector<double> bpm_results;

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

#endif // __DPM_RESULT_DIALOG_H__

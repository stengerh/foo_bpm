#ifndef __BPM_PREFERENCES_PAGE_H__
#define __BPM_PREFERENCES_PAGE_H__

#include "foobar2000/SDK/foobar2000.h"
#include "foobar2000/ATLHelpers/ATLHelpers.h"

#include "resource.h"

class bpm_preferences_page : public preferences_page_instance, public CAxDialogImpl<bpm_preferences_page>
{
	public:
		bpm_preferences_page(preferences_page_callback::ptr callback) : m_callback(callback) {}

		/***** CAxDialogImpl *****/
		enum { IDD = IDD_BPM_PREFERENCES };

		/***** preferences_page *****/
		t_uint32 get_state();
		void apply();
		void reset();

		BEGIN_MSG_MAP_EX(bpm_preferences_page)
			MSG_WM_INITDIALOG(OnInitDialog)
			// General
			COMMAND_HANDLER_EX(ID_CONFIG_BPM_PRECISION, CBN_SELCHANGE, OnComboBoxChange)
			COMMAND_HANDLER_EX(ID_CONFIG_BPM_TAG, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_AUTO_WRITE_TAG, BN_CLICKED, OnBnClicked)
			// Auto
			COMMAND_HANDLER_EX(ID_CONFIG_SECONDS_TO_READ, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_NUM_BPMS_TO_CALC, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_OFFSET_PCT_MIN, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_OFFSET_PCT_MAX, EN_CHANGE, OnEditControlChange)			
			COMMAND_HANDLER_EX(ID_CONFIG_FFT_WINDOW_SIZE, CBN_SELCHANGE, OnComboBoxChange)
			COMMAND_HANDLER_EX(ID_CONFIG_FFT_WINDOW_SLIDE, CBN_SELCHANGE, OnComboBoxChange)
			COMMAND_HANDLER_EX(ID_CONFIG_FFT_WINDOW_TYPE, CBN_SELCHANGE, OnComboBoxChange)
			COMMAND_HANDLER_EX(ID_CONFIG_BPM_MIN, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_BPM_MAX, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_INTERPOLATE_FLUX, BN_CLICKED, OnBnClicked)
			COMMAND_HANDLER_EX(ID_CONFIG_CANDIDATE_SELECTION, CBN_SELCHANGE, OnComboBoxChange)
			COMMAND_HANDLER_EX(ID_CONFIG_OUTPUT_DEBUG, BN_CLICKED, OnBnClicked)
			// Manual
			COMMAND_HANDLER_EX(ID_CONFIG_TAPS_TO_AVERAGE, EN_CHANGE, OnEditControlChange)
			COMMAND_HANDLER_EX(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, EN_CHANGE, OnEditControlChange)
		END_MSG_MAP()

	private:
		BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
		void OnEditControlChange(UINT uNotifyCode, int nID, CWindow wndCtl);
		void OnComboBoxChange(UINT uNotifyCode, int nID, CWindow wndCtl);
		void OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl);
		bool HasChanged();
		void OnChanged();

		const preferences_page_callback::ptr m_callback;
};

class bpm_preferences_page_impl : public preferences_page_impl<bpm_preferences_page>
{
	// preferences_page_impl<> helper deals with instantiation of our dialog; inherits from preferences_page_v3.
	public:
		const char* get_name();
		GUID get_guid();
		GUID get_parent_guid();
};

#endif // __BPM_PREFERENCES_PAGE_H__

#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include "resource.h"
#include "globals.h"
#include "guid.h"

#include "../SDK/foobar2000.h"
#include "../ATLHelpers/ATLHelpers.h"

typedef enum
{
	BPM_PRECISION_1 = 0,
	BPM_PRECISION_1DP,
	BPM_PRECISION_2DP
} bpm_precision_enum;

typedef enum
{
	FFT_SIZE_64 = 0,
	FFT_SIZE_128,
	FFT_SIZE_256,
	FFT_SIZE_512,
	FFT_SIZE_1024,
	FFT_SIZE_2048,
	FFT_SIZE_4096,
	FFT_SIZE_8192
} fft_window_size_enum;

typedef enum
{
	FFT_SLIDE_64 = 0,
	FFT_SLIDE_128,
	FFT_SLIDE_256,
	FFT_SLIDE_512,
	FFT_SLIDE_1024,
	FFT_SLIDE_2048,
	FFT_SLIDE_4096,
	FFT_SLIDE_8192,
	FFT_SLIDE_AUTO
} fft_window_slide_enum;

typedef enum
{
	FFT_WINDOW_HAMMING = 0,
	FFT_WINDOW_HANNING,
	FFT_WINDOW_GAUSIAN,
	FFT_WINDOW_RECT
} fft_window_enum;

typedef enum
{
	BPM_MODE = 0,
	BPM_MEDIAN,
	BPM_MEAN
} bpm_selection;

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

#endif
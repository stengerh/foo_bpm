#include "stdafx.h"

#include "bpm_preferences_page.h"

#include "preferences.h"

void bpm_preferences_page::reset()
{
	CComboBox bpm_combo_box;

	// General
	// This list MUST match the enum ordering of fft_window_size_enum
	bpm_combo_box = GetDlgItem(ID_CONFIG_BPM_PRECISION);
	bpm_combo_box.SetCurSel(BPM_PRECISION_1);
	SetDlgItemText(ID_CONFIG_BPM_TAG, _T("BPM"));
	CheckDlgButton(ID_CONFIG_AUTO_WRITE_TAG, BST_UNCHECKED);

	// Auto
	SetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, 10, true);
	SetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, 10, true);
	SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, 20, true);
	SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, 80, true);

	bpm_config_fft_window_size = FFT_SIZE_2048;
	bpm_config_fft_window_slide = FFT_SLIDE_AUTO;
	bpm_config_fft_window_type = FFT_WINDOW_HAMMING;

	// This list MUST match the enum ordering of fft_window_size_enum
	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SIZE);
	bpm_combo_box.SetCurSel(FFT_SIZE_2048);

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SLIDE);
	bpm_combo_box.SetCurSel(FFT_SLIDE_AUTO);

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_TYPE);
	bpm_combo_box.SetCurSel(FFT_WINDOW_HAMMING);

	SetDlgItemInt(ID_CONFIG_BPM_MIN, 75, true);
	SetDlgItemInt(ID_CONFIG_BPM_MAX, 195, true);
	CheckDlgButton(ID_CONFIG_INTERPOLATE_FLUX, BST_CHECKED);

	bpm_combo_box = GetDlgItem(ID_CONFIG_CANDIDATE_SELECTION);
	bpm_combo_box.SetCurSel(BPM_MODE);

	CheckDlgButton(ID_CONFIG_OUTPUT_DEBUG, BST_UNCHECKED);

	// Manual
	SetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, 30, true);
	SetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, 5, true);
}

BOOL bpm_preferences_page::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	wchar_t w_bpm_tag[256];

	// General
	// This list MUST match the enum ordering of fft_window_size_enum
	CComboBox bpm_precision_box = GetDlgItem(ID_CONFIG_BPM_PRECISION);
	bpm_precision_box.AddString(_T("Nearest 1"));
	bpm_precision_box.AddString(_T("1 Decimal"));
	bpm_precision_box.AddString(_T("2 Decimals"));
	bpm_precision_box.SetCurSel(bpm_config_bpm_precision);
	pfc::stringcvt::convert_ansi_to_wide(w_bpm_tag, 256, bpm_config_bpm_tag.get_ptr(), bpm_config_bpm_tag.length());
	SetDlgItemText(ID_CONFIG_BPM_TAG, w_bpm_tag);
	CheckDlgButton(ID_CONFIG_AUTO_WRITE_TAG, bpm_config_auto_write_tag ? BST_CHECKED : BST_UNCHECKED);

	// Auto
	SetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, bpm_config_seconds_to_read, true);
	SetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, bpm_config_num_bpms_to_calc, true);
	SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, bpm_config_offset_pct_min, true);
	SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, bpm_config_offset_pct_max, true);
	
	// This list MUST match the enum ordering of fft_window_size_enum
	CComboBox fft_window_size_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SIZE);
	fft_window_size_box.AddString(_T("64"));
	fft_window_size_box.AddString(_T("128"));
	fft_window_size_box.AddString(_T("256"));
	fft_window_size_box.AddString(_T("512"));
	fft_window_size_box.AddString(_T("1024"));
	fft_window_size_box.AddString(_T("2048"));
	fft_window_size_box.AddString(_T("4096"));
	fft_window_size_box.AddString(_T("8192"));
	fft_window_size_box.SetCurSel(bpm_config_fft_window_size);

	CComboBox fft_window_slide_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SLIDE);
	fft_window_slide_box.AddString(_T("64"));
	fft_window_slide_box.AddString(_T("128"));
	fft_window_slide_box.AddString(_T("256"));
	fft_window_slide_box.AddString(_T("512"));
	fft_window_slide_box.AddString(_T("1024"));
	fft_window_slide_box.AddString(_T("2048"));
	fft_window_slide_box.AddString(_T("4096"));
	fft_window_slide_box.AddString(_T("8192"));
	fft_window_slide_box.AddString(_T("Auto"));
	fft_window_slide_box.SetCurSel(bpm_config_fft_window_slide);

	CComboBox fft_window_type_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_TYPE);
	fft_window_type_box.AddString(_T("Hamming"));
	fft_window_type_box.AddString(_T("Hanning"));
	fft_window_type_box.AddString(_T("Gausian"));
	fft_window_type_box.AddString(_T("Rectangular (None)"));
	fft_window_type_box.SetCurSel(bpm_config_fft_window_type);

	SetDlgItemInt(ID_CONFIG_BPM_MIN, bpm_config_bpm_min, true);
	SetDlgItemInt(ID_CONFIG_BPM_MAX, bpm_config_bpm_max, true);
	CheckDlgButton(ID_CONFIG_INTERPOLATE_FLUX, bpm_config_interpolate_flux ? BST_CHECKED : BST_UNCHECKED);

	CComboBox candidate_selection_box = GetDlgItem(ID_CONFIG_CANDIDATE_SELECTION);
	candidate_selection_box.AddString(_T("Mode"));
	candidate_selection_box.AddString(_T("Median"));
	candidate_selection_box.AddString(_T("Mean"));
	candidate_selection_box.SetCurSel(bpm_config_candidate_selection);

	CheckDlgButton(ID_CONFIG_OUTPUT_DEBUG, bpm_config_output_debug ? BST_CHECKED : BST_UNCHECKED);

	// Manual
	SetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, bpm_config_taps_to_average, true);
	SetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, bpm_config_seconds_to_reset_average, true);

	return TRUE;
}

void bpm_preferences_page::OnEditControlChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID)
	{
		case ID_CONFIG_BPM_TAG:
			break;
		case ID_CONFIG_SECONDS_TO_READ:
			if (GetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, 1, true);
			}
			break;
		case ID_CONFIG_NUM_BPMS_TO_CALC:
			if (GetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, 1, true);
			}
			break;
		case ID_CONFIG_OFFSET_PCT_MIN:
			if (GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, 1, true);
			}
			else if (GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, NULL, false) > 99)
			{
				SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, 99, true);
			}
			break;
		case ID_CONFIG_OFFSET_PCT_MAX:
			if (GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, 1, true);
			}
			else if (GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, NULL, false) > 99)
			{
				SetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, 99, true);
			}
			break;
		case ID_CONFIG_BPM_MIN:
			if (GetDlgItemInt(ID_CONFIG_BPM_MIN, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_BPM_MIN, 1, true);
			}
			else if (GetDlgItemInt(ID_CONFIG_BPM_MIN, NULL, false) > 500)
			{
				SetDlgItemInt(ID_CONFIG_BPM_MIN, 500, true);
			}
			break;
		case ID_CONFIG_BPM_MAX:
			if (GetDlgItemInt(ID_CONFIG_BPM_MAX, NULL, false) > 500)
			{
				SetDlgItemInt(ID_CONFIG_BPM_MAX, 500, true);
			}
			else if (GetDlgItemInt(ID_CONFIG_BPM_MAX, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_BPM_MAX, 1, true);
			}			
			break;
		case ID_CONFIG_TAPS_TO_AVERAGE:
			if (GetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, 1, true);
			}
			break;
		case ID_CONFIG_SECONDS_TO_RESET_AVERAGE:
			if (GetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, NULL, false) < 1)
			{
				SetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, 1, true);
			}
		default:
			break;
	}

	OnChanged();
}

void bpm_preferences_page::OnComboBoxChange(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID)
	{
		case ID_CONFIG_BPM_PRECISION:
			break;
		case ID_CONFIG_FFT_WINDOW_SIZE:
			break;
		case ID_CONFIG_FFT_WINDOW_SLIDE:
			break;
		case ID_CONFIG_FFT_WINDOW_TYPE:
			break;
		case ID_CONFIG_CANDIDATE_SELECTION:
			break;
		default:
			break;
	}

	OnChanged();
}

void bpm_preferences_page::OnBnClicked(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID)
	{
		case ID_CONFIG_INTERPOLATE_FLUX:
			break;
		case ID_CONFIG_OUTPUT_DEBUG:
			break;
		case ID_CONFIG_AUTO_WRITE_TAG:
			break;
		default:
			break;
	}

	OnChanged();
}

void bpm_preferences_page::apply()
{
	CComboBox bpm_combo_box;
	char a_bpm_tag[256];
	wchar_t w_bpm_tag[256];
	unsigned w_length = 0;
	unsigned a_length = 0;

	w_length = GetDlgItemText(ID_CONFIG_BPM_TAG, w_bpm_tag, 256) + 1;
	a_length = pfc::stringcvt::estimate_wide_to_ansi(w_bpm_tag, w_length);
	bpm_config_bpm_tag.prealloc(a_length);
	pfc::stringcvt::convert_wide_to_ansi(a_bpm_tag, a_length, w_bpm_tag, w_length);
	bpm_config_bpm_tag = a_bpm_tag;

	bpm_config_seconds_to_read = GetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, NULL, false);
	bpm_config_num_bpms_to_calc = GetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, NULL, false);
	bpm_config_offset_pct_min = GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, NULL, false);
	bpm_config_offset_pct_max = GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, NULL, false);
	bpm_config_bpm_min = GetDlgItemInt(ID_CONFIG_BPM_MIN, NULL, false);
	bpm_config_bpm_max = GetDlgItemInt(ID_CONFIG_BPM_MAX, NULL, false);
	bpm_config_taps_to_average = GetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, NULL, false);
	bpm_config_seconds_to_reset_average = GetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, NULL, false);
	
	bpm_combo_box = GetDlgItem(ID_CONFIG_BPM_PRECISION);
	bpm_config_bpm_precision = bpm_combo_box.GetCurSel();

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SIZE);
	bpm_config_fft_window_size = bpm_combo_box.GetCurSel();

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SLIDE);
	bpm_config_fft_window_slide = bpm_combo_box.GetCurSel();

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_TYPE);
	bpm_config_fft_window_type = bpm_combo_box.GetCurSel();

	bpm_combo_box = GetDlgItem(ID_CONFIG_CANDIDATE_SELECTION);
	bpm_config_candidate_selection = bpm_combo_box.GetCurSel();

	bpm_config_interpolate_flux = (IsDlgButtonChecked(ID_CONFIG_INTERPOLATE_FLUX) == BST_CHECKED);
	bpm_config_output_debug = (IsDlgButtonChecked(ID_CONFIG_OUTPUT_DEBUG) == BST_CHECKED);
	bpm_config_auto_write_tag = (IsDlgButtonChecked(ID_CONFIG_AUTO_WRITE_TAG) == BST_CHECKED);
}

t_uint32 bpm_preferences_page::get_state()
{
	t_uint32 state = preferences_state::resettable;

	if (HasChanged())
	{
		state |= preferences_state::changed;
	}

	return state;
}

bool bpm_preferences_page::HasChanged()
{
	bool changed = false;

	CComboBox bpm_combo_box;
	char a_bpm_tag[256];
	wchar_t w_bpm_tag[256];
	unsigned w_length = 0;
	unsigned a_length = 0;

	w_length = GetDlgItemText(ID_CONFIG_BPM_TAG, w_bpm_tag, 256) + 1;
	a_length = pfc::stringcvt::estimate_wide_to_ansi(w_bpm_tag, w_length);
	bpm_config_bpm_tag.prealloc(a_length);
	pfc::stringcvt::convert_wide_to_ansi(a_bpm_tag, a_length, w_bpm_tag, w_length);
	if (strcmp(bpm_config_bpm_tag, a_bpm_tag) != 0 ||
		bpm_config_seconds_to_read != GetDlgItemInt(ID_CONFIG_SECONDS_TO_READ, NULL, false) ||
		bpm_config_num_bpms_to_calc != GetDlgItemInt(ID_CONFIG_NUM_BPMS_TO_CALC, NULL, false) ||
		bpm_config_offset_pct_min != GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MIN, NULL, false) ||
		bpm_config_offset_pct_max != GetDlgItemInt(ID_CONFIG_OFFSET_PCT_MAX, NULL, false) ||
		bpm_config_bpm_min != GetDlgItemInt(ID_CONFIG_BPM_MIN, NULL, false) ||
		bpm_config_bpm_max != GetDlgItemInt(ID_CONFIG_BPM_MAX, NULL, false) ||
		bpm_config_taps_to_average != GetDlgItemInt(ID_CONFIG_TAPS_TO_AVERAGE, NULL, false) ||
		bpm_config_seconds_to_reset_average != GetDlgItemInt(ID_CONFIG_SECONDS_TO_RESET_AVERAGE, NULL, false) ||
		bpm_config_interpolate_flux != (IsDlgButtonChecked(ID_CONFIG_INTERPOLATE_FLUX) == BST_CHECKED) ||
		bpm_config_output_debug != (IsDlgButtonChecked(ID_CONFIG_OUTPUT_DEBUG) == BST_CHECKED) ||
		bpm_config_auto_write_tag != (IsDlgButtonChecked(ID_CONFIG_AUTO_WRITE_TAG) == BST_CHECKED))
	{
		changed = true;
	}

	bpm_combo_box = GetDlgItem(ID_CONFIG_BPM_PRECISION);
	if (bpm_config_bpm_precision != bpm_combo_box.GetCurSel())
	{
		changed = true;
	}

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SIZE);
	if (bpm_config_fft_window_size != bpm_combo_box.GetCurSel())
	{
		changed = true;
	}

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_SLIDE);
	if (bpm_config_fft_window_slide != bpm_combo_box.GetCurSel())
	{
		changed = true;
	}

	bpm_combo_box = GetDlgItem(ID_CONFIG_FFT_WINDOW_TYPE);
	if (bpm_config_fft_window_type != bpm_combo_box.GetCurSel())
	{
		changed = true;
	}

	bpm_combo_box = GetDlgItem(ID_CONFIG_CANDIDATE_SELECTION);
	if (bpm_config_candidate_selection != bpm_combo_box.GetCurSel())
	{
		changed = true;
	}

	return changed;
}
void bpm_preferences_page::OnChanged()
{
	m_callback->on_state_changed();
}






const char * bpm_preferences_page_impl::get_name()
{
	return "BPM Auto Analysis";
}

GUID bpm_preferences_page_impl::get_guid()
{
	return guid_bpm_preferences;
}

GUID bpm_preferences_page_impl::get_parent_guid()
{
	return preferences_page::guid_tools;
}

static preferences_page_factory_t<bpm_preferences_page_impl> g_bpm_preferences_page_impl;

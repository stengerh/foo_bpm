#ifndef __CONTEXT_MENU_H__
#define __CONTEXT_MENU_H__

#include "foobar2000/SDK/foobar2000.h"

class contextmenu_item_simple_bpm : public contextmenu_item_simple
{
public:
	GUID get_parent();
	unsigned get_num_items();
	void get_item_name(unsigned p_index, pfc::string_base & p_out);
	void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller);
	GUID get_item_guid(unsigned p_index);
	bool get_item_description(unsigned p_index,pfc::string_base & p_out);

private:
	enum menu_index
	{
		mnuAutoAnalysis,
		mnuManualAnalysis,
		mnuDoubleBPM,
		mnuHalveBPM,
		mnuTotal
	};

	void run_auto_analysis(metadb_handle_list_cref p_data);
	void run_manual_analysis(metadb_handle_list_cref p_data);
	void run_double_or_halve_bpm(metadb_handle_list_cref p_data, bool p_halve);
};

#endif
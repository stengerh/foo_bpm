#ifndef __CONTEXT_MENU_H__
#define __CONTEXT_MENU_H__

#include "guid.h"
#include "foo_bpm.h"

#include "../SDK/foobar2000.h"

class contextmenu_item_simple_bpm : public contextmenu_item_simple
{
	private:
		enum menu_index
		{
			mnuAutoAnalysis,
			mnuManualAnalysis,
			mnuDoubleBPM,
			mnuHalveBPM,
			mnuTotal
		};

		GUID get_parent();
		unsigned get_num_items();
		void get_item_name(unsigned p_index, pfc::string_base & p_out);
		void context_command(unsigned p_index, metadb_handle_list_cref p_data, const GUID& p_caller);
		GUID get_item_guid(unsigned p_index);
		bool get_item_description(unsigned p_index,pfc::string_base & p_out);
};

#endif
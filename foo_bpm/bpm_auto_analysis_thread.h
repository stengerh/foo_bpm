#ifndef __BPM_AUTO_ANALYSIS_THREAD_H__
#define __BPM_AUTO_ANALYSIS_THREAD_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"

class bpm_auto_analysis_thread : public threaded_process_callback
{
	public:
		bpm_auto_analysis_thread(metadb_handle_list_cref playlist_items);
		void start();

	private:
		pfc::list_t<metadb_handle_ptr> p_data;
		pfc::list_t<file_info_impl> p_info;
		int p_data_size;
		std::vector<double> bpm_results;
		void run(threaded_process_status & p_status, abort_callback & p_abort);
		void on_done(HWND p_wnd, bool p_was_aborted);
};

#endif // __BPM_AUTO_ANALYSIS_THREAD_H__

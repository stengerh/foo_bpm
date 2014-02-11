#ifndef __BPM_AUTO_ANALYSIS_THREAD_H__
#define __BPM_AUTO_ANALYSIS_THREAD_H__

#include <vector>

#include "foobar2000/SDK/foobar2000.h"

class bpm_auto_analysis_thread : public threaded_process_callback
{
	public:
		bpm_auto_analysis_thread(metadb_handle_list_cref p_tracks);
		void start();

	private:
		void run(threaded_process_status & p_status, abort_callback & p_abort);
		void on_done(HWND p_wnd, bool p_was_aborted);

		pfc::list_t<metadb_handle_ptr> m_tracks;
		pfc::list_t<file_info_impl> m_infos;
		std::vector<double> m_bpm_results;
};

#endif // __BPM_AUTO_ANALYSIS_THREAD_H__

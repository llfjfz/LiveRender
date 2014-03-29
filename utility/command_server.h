#ifndef __COMMAND_SERVER__
#define __COMMAND_SERVER__
#include "command_recorder.h"
#include "cache.hpp"

class CommandServer : public Network, public Buffer {
public:
	CommandServer();
	CommandServer(int size_limit);
	~CommandServer();

	Buffer* dump_buffer();

	void write_vec(int op_code, float* vec, int size=16);

	void init();
	void start_up();
	void shut_down();

	void accept_client();
	void begin_command(int op_code, int id);
	void end_command(int force_flush = 0);
	void cancel_command();
	int get_command_length();
	void print_record();

	int flush();

	ServerConfig* config_;

private:
	int op_code;
	int obj_id;
	int sv_obj_id;
	char* sv_ptr;
	char* cm_len_ptr;

	char* rid_pos;
	
	short func_count_;
	int size_limit_;

	CommandRecorder* cr_;
	
	Cache* cache_mgr_;
};

#endif
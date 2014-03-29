#ifndef __COMMAND_CLIENT__
#define __COMMAND_CLIENT__

#include "utility.h"
#include <iostream>
using namespace std;

#include "client_cache.hpp"

struct Command {
	int op_code_;
	char* offest_;
	int length_;

	Command(int op_code, char* offest, int length): op_code_(op_code), offest_(offest), length_(length) {

	}
};

class CommandClient : public Network, public Buffer {
public:
	CommandClient();
	~CommandClient();

	void load_port(int client_num);
	void init();

	void read_vec(float* vec, int size=16);
	
	void record_last_command();
	bool validate_last_command();
	void take_command(int& op_code, int& obj_id);

private:
	char* sv_ptr;
	short func_count;

	int op_code_;
	int obj_id_;

	ClientCache* cache_mgr_;
	ClientConfig* config_;
	Command* last_command_;
};

#endif


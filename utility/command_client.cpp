#include "utility.h"

CommandClient::CommandClient(): func_count(0), op_code_(0), obj_id_(0) {
	config_ = new ClientConfig("game_client.conf");
	

	sv_ptr = NULL;

	cache_mgr_ = new ClientCache();
	last_command_ = new Command(-1, get_buffer() + 6, 0);
	set_cache_filter();
}

void CommandClient::load_port(int client_num) {
	if(client_num < 1 || client_num > 8) {
		printf("client_num should be among [1-8]\n");
		return;
	}
	config_->load_config(client_num);
}

void CommandClient::init() {
	if(!init_socket()) return;

	connect_socket = socket(AF_INET, SOCK_STREAM, 0);
	const char ch_opt = 1;
	int err = setsockopt(connect_socket, IPPROTO_TCP, TCP_NODELAY, &ch_opt, sizeof(char));

	SOCKADDR_IN addr_in;
	memset(&addr_in, 0, sizeof(SOCKADDR_IN));
	addr_in.sin_addr.S_un.S_addr = inet_addr(config_->srv_ip_);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(config_->srv_port_);

	int connect_ret_val = connect(connect_socket, (SOCKADDR*)&addr_in, sizeof(SOCKADDR));
}

CommandClient::~CommandClient() {
	close_socket(connect_socket);
	clean_up();
}

bool CommandClient::validate_last_command() {
	if(last_command_->op_code_ == -1) return true;

	int consume_length = get_cur_ptr() - last_command_->offest_;

	if(consume_length != last_command_->length_) {
		Log::slog("=========================================================================\n");
		Log::slog("The following command has errors, please check your code!\n");
		Log::slog("\top_code=%d, expected length=%d, consumed length=%d\n", last_command_->op_code_, last_command_->length_, consume_length);
	}
	assert(consume_length == last_command_->length_);

	return (consume_length == last_command_->length_);
}

void CommandClient::record_last_command() {
	last_command_->offest_ = get_cur_ptr();

	char* data = get_cur_ptr(4);
	last_command_->length_ = *((int*)data);
}

void CommandClient::take_command(int& op_code, int& obj_id) {
	if(sv_ptr)
		cur_ptr = sv_ptr;

#ifdef Enable_Command_Validate
	if(!validate_last_command()) {
		op_code = obj_id = -1;
		return;
	}
#endif

	
	if(func_count == 0) {
		int len = recv_packet(this);

		if(len <= 0) {
			op_code = obj_id = -1;
			return;
		}

		//cout << "len part: " << get_length_part() << endl;

		func_count = get_count_part();
		//cout << "count: " << func_count << endl;
		move_over(2);
	}

	--func_count;

#ifdef Enable_Command_Validate
	record_last_command();
#endif

	op_code = read_uchar();
	if(op_code & 1) {
		obj_id = read_ushort();
	}
	else {
		obj_id = 0;
	}
	op_code >>= 1;

#ifdef USE_CACHE

	if(cache_filter[op_code]) {
		int cache_id = read_ushort();
		
		if((cache_id & 1) == Cache_Use) {
			Log::log("cache hited\n");
			sv_ptr = cur_ptr;
			cur_ptr = cache_mgr_->get_cache(cache_id >> 1);
		}
		else {
			//set cache
			Log::log("cache miss\n");
			sv_ptr = NULL;
			cache_mgr_->set_cache((cache_id >> 1), get_cur_ptr(), 200);
		}
	}
	else {
		sv_ptr = NULL;
	}

#endif

	last_command_->op_code_ = op_code;
}

void CommandClient::read_vec(float* vec, int size) {
	int cache_id = read_ushort();
	if((cache_id & 1) == Cache_Use) {
		char* p = cache_mgr_->get_cache(cache_id >> 1);
		memcpy((char*)vec, p, size);  //size = 16 or 8
	}
	else {
		read_byte_arr((char*)vec, size);
		cache_mgr_->set_cache((cache_id >> 1), (char*)vec, size);
	}
}


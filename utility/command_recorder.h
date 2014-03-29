#ifndef __COMMAND_RECORDER__
#define __COMMAND_RECORDER__

struct RecordType {
	int id, count;
	long long length;
	int hit_cnt, miss_cnt;
};

class CommandRecorder {
public:
	CommandRecorder();
	void add_record(int op_code, int length);
	void add_lengh(int length);
	void print_info();

	void cache_hit(int op_code);
	void cache_miss(int op_code);

private:
	RecordType info[1010];
	int inst_cnt;
	long long frame_cnt;
	long long frame_len;
	long long total_len;
	long long cmd_cnt;
};


#endif

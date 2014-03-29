#ifndef __COMMON_NET__
#define __COMMON_NET__

#include "utility.h"
class CommonNet : public Network{
private:
	int type;
	char * buffer;

	char * cur_ptr;
	int buf_size;

	ServerConfig* config_server;
	ClientConfig * config_client;
public:
	CommonNet(int type, int clien_num = 1);

	~CommonNet();

	void set_config(ServerConfig * config){
		config_server = config;
	}
	void set_config(ClientConfig * config){
		config_client = config;
	}
	bool init_server(int port);
	bool init_client(int port);
	//Buffer* dump_buffer();

	void ConnectServer(int port);
	void AcceptClient(int port);

	void write_int(int);
	void write_uint(UINT);
	void write_char(char);
	void write_uchar(UCHAR);
	void write_float(float);
	void write_short(short);
	void write_ushort(USHORT);
	void write_byte_arr(char *, int );

	int read_int();
	UINT read_uint();
	char read_char();
	unsigned char read_uchar();
	float read_float();
	short read_short();
	USHORT read_ushort();
	void read_byte_arr(char * dst, int len);

	char * get_cur_ptr(int len);
	void begin_transaction();
	void end_transaction();
	int get_data();


};


#endif
#ifndef __NETWORK__
#define __NETWORK__

class Buffer;

class Network {
protected:
	Network();
	~Network();

	bool init_socket();
	void close_socket(SOCKET socket);
	void clean_up();
	void Accept();

	void set_cache_filter();

	int send_packet(Buffer* buffer);
	int recv_packet(Buffer* buffer);
	int recv_n_byte(char* buffer, int nbytes);

	SOCKET connect_socket;
	SOCKET listen_socket;

	bool cache_filter[MaxSizeUntilNow_Opcode + 10];

public:
	SOCKET& get_connect_socket();
	SOCKET& get_listen_socket();
	void set_connect_socket(const SOCKET connect_socket);

	void send_raw_buffer(string buf);
	void send_raw_buffer(char * buf, int len);
	void recv_raw_buffer(string& buf, int& len);
	int recv_raw_buffer(char * buf, int len);
	//void send_raw_buffer(char* buf, int len);
	//void recv_raw_buffer(char* dst, int len);
};

#endif



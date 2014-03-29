#include "utility.h"

char tmp_buffer[100000];

Network::Network() {

}

Network::~Network() {

}

void Network::set_cache_filter() {
	memset(cache_filter, 0, sizeof cache_filter);
	cache_filter[SetStreamSource_Opcode] = 1;
	cache_filter[DrawIndexedPrimitive_Opcode] = 1;
	cache_filter[SetRenderState_Opcode] = 1;
	cache_filter[SetRenderTarget_Opcode] = 1;
	cache_filter[SetTexture_Opcode] = 1;

	cache_filter[SetPixelShader_Opcode] = 1;
	cache_filter[SetVertexShader_Opcode] = 1;
	cache_filter[SetSamplerState_Opcode] = 1;
	cache_filter[SetViewport_Opcode] = 1;
}

int Network::send_packet(Buffer* buffer) {
	if(connect_socket == -1) return -1;

#ifndef ENABLE_NETWORK_COMPRESS
	buffer->set_length_part();
	int len = send(connect_socket, buffer->get_buffer(), buffer->get_size(), 0);

	//printf("Network::send_packet(), %d bytes sent.\n", len);
	return len;
#else

	int out_len = 4;
	//Log::log("Network::send_packet(), buffer size=%d\n", buffer->get_size());
	Compressor::lzo_encode(buffer->get_buffer() + 4, buffer->get_size() - 4, buffer->com_buffer_ + 4, out_len);
	out_len += 4;
	memcpy(buffer->com_buffer_, &out_len, 4);

	int len = send(connect_socket, buffer->com_buffer_, out_len, 0);

	//Log::log("Network::send_packet(), %d bytes send\n", len);

	return len;

#endif
}

int Network::recv_packet(Buffer* buffer) {
	if(connect_socket == -1) return -1;

#ifndef ENABLE_NETWORK_COMPRESS
	int k;
	k = recv_n_byte(buffer->get_buffer(), 4);

	if(k <= 0) return k;

	int total_len = buffer->get_length_part();
	k = recv_n_byte(buffer->get_buffer() + 4, total_len - 4);

	if(k <= 0) return k;
	buffer->go_back(buffer->get_buffer() + 4);

	return total_len;
#else

	int k;
	k = recv_n_byte(buffer->com_buffer_, 4);
	if(k <= 0) return k;

	int total_len = *((int*)(buffer->com_buffer_));
	int x = recv_n_byte(buffer->com_buffer_ + 4, total_len - 4);

	int out_len = 0;
	Compressor::lzo_decode(buffer->com_buffer_ + 4, total_len - 4, buffer->get_buffer() + 4, out_len);

	buffer->go_back(buffer->get_buffer() + 4);

	return out_len + 4;

#endif

}

int Network::recv_n_byte(char* buffer, int nbytes) {
	int recvlen = 0;

	while(recvlen != nbytes) {
		int t = recv(connect_socket, buffer + recvlen, nbytes - recvlen, 0);

		if(t <= 0) return t;
		
		recvlen += t;
	}

	return recvlen;
}

void Network::send_raw_buffer(string buf) {
	if(connect_socket == -1) return;

	int len = send(connect_socket, buf.c_str(), buf.length() + 1, 0);
}
void Network::send_raw_buffer(char * buf, int len){
	if(connect_socket == -1)return;

	int len1 = send(connect_socket, buf, len, 0);
	//Log::slog("send size:%d\n", len1);
}

void Network::recv_raw_buffer(string& buf, int& len) {
	if(connect_socket == -1) return;

	char tmp[100 + 10];
	len = recv(connect_socket, tmp, 100, 0);
	buf = tmp;
}
int Network::recv_raw_buffer(char * buf, int len){
	if( connect_socket == -1) return 0;

	//char tmp[10000];
	return recv(connect_socket, buf, len, 0);
}

bool Network::init_socket() {
	WORD version;
	WSAData wsaData;
	version = MAKEWORD(1, 1);

	int err = WSAStartup(version, &wsaData);
	if(err) {
		printf("Network::init_socket(), socket start failed\n");
		return false;
	}
	else {
		printf("Network::init_socket(), socket start success\n");
		return true;
	}
}

void Network::Accept() {
	SOCKADDR_IN addr_in;
	memset(&addr_in, 0, sizeof(SOCKADDR_IN));
	int len = sizeof(SOCKADDR);
	printf("Network::Accept() called\n");
	connect_socket = accept(listen_socket, (SOCKADDR*)&addr_in, &len);
	printf("Network::Accept() end\n");
}

void Network::clean_up() {
	WSACleanup();
}

void Network::close_socket(SOCKET socket) {
	closesocket(socket);
}

SOCKET& Network::get_connect_socket() {
	return this->connect_socket;
}

SOCKET& Network::get_listen_socket() {
	return this->listen_socket;
}

void Network::set_connect_socket(const SOCKET connect_socket) {
	this->connect_socket = connect_socket;
}


#include "common_net.h"

/* construct common net with type
* param: type, the type of the net instance, 1 for server, and 2 for client
*/
#define BUFF_LEN 1000000

CommonNet::CommonNet(int type, int client_num ){
	buffer = new char[BUFF_LEN];
	
	cur_ptr = buffer;
	if(type == 2){
		//init as client
		config_client = new ClientConfig("game_client.conf");
		config_client->load_config(client_num);
		//printf("client load port:%d\n", this->config_client->)
		//sv_ptr = NULL;
	}
	else if(type == 1){
		printf("Create common net \n");
		//InitAsServer("command_server", type);
		config_server = new ServerConfig("game_server.conf");
		config_server->load_config();
	}
}
CommonNet::~CommonNet(){
	this->close_socket(connect_socket);
	this->close_socket(listen_socket);
	clean_up();
}


bool CommonNet::init_server(int port_off){
	if(!init_socket()) return false;
	
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	const char ch_opt = 1;
	int err = setsockopt(listen_socket, IPPROTO_TCP, TCP_NODELAY, &ch_opt, sizeof(char));
	if(err == -1) {
		return false;
	}
	SOCKADDR_IN addr_in;
	memset(&addr_in, 0, sizeof(SOCKADDR_IN));
	addr_in.sin_family = AF_INET;
	addr_in.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr_in.sin_port = htons(config_server->command_port_+port_off);
	printf("server listen port:%d\n", config_server->command_port_ + port_off);

	bind(listen_socket, (SOCKADDR*)&addr_in, sizeof(SOCKADDR));
	listen(listen_socket, 5);

	return true;
}
bool CommonNet::init_client(int port_off){
	return true;
}


void CommonNet::ConnectServer(int port){
	if(!init_socket()) return;

	connect_socket = socket(AF_INET, SOCK_STREAM, 0);
	const char ch_opt = 1;
	int err = setsockopt(connect_socket, IPPROTO_TCP, TCP_NODELAY, &ch_opt, sizeof(char));

	SOCKADDR_IN addr_in;
	memset(&addr_in, 0, sizeof(SOCKADDR_IN));
	addr_in.sin_addr.S_un.S_addr = inet_addr(config_client->srv_ip_);
	addr_in.sin_family = AF_INET;
	addr_in.sin_port = htons(config_client->srv_port_ + port);

	int connect_ret_val = connect(connect_socket, (SOCKADDR*)&addr_in, sizeof(SOCKADDR));
}
void CommonNet::AcceptClient(int port){
	SOCKADDR_IN addr_in;
	memset(&addr_in, 0, sizeof(SOCKADDR_IN));
	int len = sizeof(SOCKADDR);
	printf("Network::Accept() called\n");
	connect_socket = accept(listen_socket, (SOCKADDR*)&addr_in, &len);
	printf("Network::Accept() end\n");
}

char * CommonNet::get_cur_ptr(int len){
	return this->buffer + len;
}


void CommonNet::write_int(int data) {

#ifdef USE_VARINT
	Compressor::encode_int(data, cur_ptr);
#else
	*( (int*)(cur_ptr) ) = data;
	cur_ptr += sizeof(int);
#endif
}
void CommonNet::write_uint(UINT data) {
#ifdef USE_VARINT
	Compressor::encode_uint(data, cur_ptr);
#else
	*( (UINT*)(cur_ptr) ) = data;
	cur_ptr += sizeof(UINT);
#endif
}
void CommonNet::write_char(char data) {
	*( (char*)(cur_ptr) ) = data;
	cur_ptr += sizeof(char);
}
void CommonNet::write_uchar(unsigned char data) {
	*( (unsigned char*)(cur_ptr) ) = data;
	cur_ptr += sizeof(unsigned char);
}
void CommonNet::write_float(float data) {
	*( (float*)(cur_ptr) ) = data;
	cur_ptr += sizeof(float);
}
void CommonNet::write_short(short data) {
	*( (short*)(cur_ptr) ) = data;
	cur_ptr += sizeof(short);
}
void CommonNet::write_ushort(USHORT data) {
	*( (USHORT*)(cur_ptr) ) = data;
	cur_ptr += sizeof(USHORT);
}
void CommonNet::write_byte_arr(char* data, int length) {
	memcpy(cur_ptr, data, length);
	cur_ptr += length;
}

int CommonNet::read_int() {
#ifdef USE_VARINT
	int data;
	Compressor::decode_int(cur_ptr, data);
	return data;
#else
	int data = *( (int*)(cur_ptr) );
	cur_ptr += sizeof(int);
	return data;
#endif
}
UINT CommonNet::read_uint() {
#ifdef USE_VARINT
	UINT data;
	Compressor::decode_uint(cur_ptr, data);
	return data;
#else
	UINT data = *( (UINT*)(cur_ptr) );
	cur_ptr += sizeof(UINT);
	return data;
#endif
}
char CommonNet::read_char() {
	char data = *( (char*)(cur_ptr) );
	cur_ptr += sizeof(char);
	return data;
}
UCHAR CommonNet::read_uchar() {
	UCHAR data = *( (UCHAR*)(cur_ptr) );
	cur_ptr += sizeof(UCHAR);
	return data;
}
float CommonNet::read_float() {
	float data = *( (float*)(cur_ptr) );
	cur_ptr += sizeof(float);
	return data;
}
short CommonNet::read_short() {
	short data = *( (short*)(cur_ptr) );
	cur_ptr += sizeof(short);
	return data;
}
USHORT CommonNet::read_ushort() {
	USHORT data = *( (USHORT*)(cur_ptr) );
	cur_ptr += sizeof(USHORT);
	return data;
}
void CommonNet::read_byte_arr(char* dst, int length) {
	memcpy(dst, cur_ptr, length);
	cur_ptr += length;
}
// client begin transaction to start transmit data
void CommonNet::begin_transaction(){
	cur_ptr = buffer;//+ sizeof(int);
	buf_size = 0;//sizeof(int);
}
// client end transaction to actually send data
void CommonNet::end_transaction(){
	buf_size = cur_ptr - buffer;
	//Log::slog("transaction end with data len:%d\n", buf_size);
	this->send_raw_buffer(buffer, buf_size);
	//SendPacket();
	cur_ptr = buffer;
}
// the server will get data from net
int CommonNet::get_data(){
	//int len = recv_n_byte(buffer, sizeof(buffer));
	int len = recv_raw_buffer(buffer, BUFF_LEN );
	//Log::slog("CommonNet::get_data() recv data:%d\n", len);
	cur_ptr = buffer;//+sizeof(int);
	return len;
}
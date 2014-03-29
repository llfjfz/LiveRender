#include "common_net.h"
#include <process.h>
#include "game_client.h"


int op_code;
int obj_id;

#define maxl 10010

char buffer[maxl] = "hello, world";
char b2[maxl];
char b3[maxl];

CommandClient cc;
//
CommonNet *dic = NULL;//dic(2);

bool client_render = true;
bool use_server_style = false;
string game_name = "xx";

void usage() {
	puts("\nUsage:");
	puts("\tgame_client game_name [window_style y/n] [client_id(1-8)]");
}

int main(int argc, char** argv) {

	/*
	int outlen = 0;
	Compressor::lzo_encode(buffer, strlen(buffer) + 1, b2, outlen);

	int out3;
	Compressor::lzo_decode(b2, outlen, b3, out3);
	puts(b3);
	*/
	

	init_fptable();

	
	Log::init("game_client.log");

	if(argc == 2) {
		printf("start game: %s\n", argv[1]);
		cc.load_port(1);
		dic = new CommonNet(2, 1);
	}
	else if(argc == 3) {
		printf("start game: %s\n", argv[1]);
		use_server_style = true;
		
		game_name = string(argv[1]);

		cc.load_port(1);
		dic = new CommonNet(2, 1);
	}
	else if(argc == 4) {
		printf("start game: %s\n", argv[1]);
		use_server_style = true;
		
		game_name = string(argv[1]);

		int c_id = atoi(argv[3]);

		if(c_id > 1) {
			client_render = false;
		}

		cc.load_port(c_id);
		dic = new CommonNet(2, c_id);
	}
	else {
		usage();
		return 0;
	}
	
	cc.init();

	// input client connect to the server
	dic->ConnectServer(100);

	cc.send_raw_buffer(string(argv[1]));


	// initial the section
	InitializeCriticalSection(&main_section);
	p_main_section = &main_section;
	// begin the input thread
	DWORD dwThreadId;

#ifdef USE_CLIENT_INPUT
	HANDLE hThreadInput = chBEGINTHREADEX(NULL, 0, InputThread, dic, 0, &dwThreadId);
#endif

	MSG xmsg = {0};
	while(xmsg.message != WM_QUIT) {
		if(PeekMessage(&xmsg, NULL, 0U, 0U, PM_REMOVE)) {
			TranslateMessage(&xmsg);
			DispatchMessage(&xmsg);
		}
		else {
			cc.take_command(op_code, obj_id);
			Log::log("op_code=%d, obj_id=%d\n", op_code, obj_id);
			
			if(op_code >= 0 && op_code < MaxSizeUntilNow_Opcode) {
				if(client_render)
					(*(funcs[op_code].func))();
			}
			else {
				if(op_code == MaxSizeUntilNow_Opcode) {
					Log::slog("game_client exit normally.");
				}
				else {
					Log::slog("game client exit, unexpected op_code");
				}
				break;
			}
		}
	}
	EnterCriticalSection(p_main_section);
	main_thread_running = false;
	LeaveCriticalSection(p_main_section);

	cc.~CommandClient();
	dic->~CommonNet();

	return 0;
}

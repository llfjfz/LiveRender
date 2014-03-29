//#include "utility.h"
#include "common_net.h"
#include "detours/detours.h"

#pragma comment(lib, "detours/detours.lib")

int main(int argc, char** argv) {
	CommandServer cs(1);
	cs.init();

	CommonNet dis(1);
	dis.init_server(100);

	PROCESS_INFORMATION pi = {0};
	STARTUPINFO si = {0};
	si.cb = sizeof(si);

	HANDLE hnd = GetCurrentProcess();

	printf("Listening\nhandle %d\n", hnd);

	LPSECURITY_ATTRIBUTES lp_attributes;
	LPSECURITY_ATTRIBUTES lpThreadAttributes;
	STARTUPINFO startupInfo = {sizeof(startupInfo)};
	memset(&startupInfo,0,sizeof(STARTUPINFO));
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags=0;
	startupInfo.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION processInformation;
	char cmdLine[100];
	string AppName;
	int recv_len = 0;
	char RecvB[100];

	if(argc == 2 || argc == 3) {
		//启动游戏进程
		DWORD id = GetCurrentProcessId();

		int dump_mesh = 0;
		if(argc == 3) dump_mesh = 1;

		sprintf(cmdLine,"%s %d %d %d %d",argv[1], dump_mesh, -1,-1, id);
		printf("cmd line is %s\n", cmdLine);

		bool ret = DetourCreateProcessWithDll(NULL,cmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE,
			NULL, NULL, &si, &pi, "game_server.dll", NULL);

		if(!ret) {
			char err_str[200];
			sprintf(err_str, "Game Start %s Failed", AppName.c_str());
			MessageBox(NULL, err_str, "Error", MB_OK);
		}

		return 0;
	}

	while(true) {
		cs.accept_client();
		printf("client has come with connect socket:%d\n", cs.get_connect_socket());

		dis.AcceptClient(1);
		printf("input client connect to socket:%d\n", dis.get_connect_socket());
		
		cs.recv_raw_buffer(AppName, recv_len);
		if(recv_len <= 0) {
			printf("recv error\n");
			break;
		}
		printf("Appname: %s\n", AppName.c_str());


		//启动游戏进程
		DWORD id = GetCurrentProcessId();
		sprintf(cmdLine,"%s %d %d %d %d",AppName.c_str(), 0, cs.get_connect_socket(), dis.get_connect_socket(), id);
		printf("cmd line is %s\n", cmdLine);
		bool ret = DetourCreateProcessWithDll(NULL,cmdLine, NULL, NULL, TRUE, CREATE_DEFAULT_ERROR_MODE,
			NULL, NULL, &si, &pi, "game_server.dll", NULL);

		if(!ret) {
			char err_str[200];
			sprintf(err_str, "Game Start %s Failed", AppName.c_str());
			MessageBox(NULL, err_str, "Error", MB_OK);
		}
	}

	return 0;
}



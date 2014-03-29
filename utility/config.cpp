#include "utility.h"

/////////////////////////////////////////////////////////////////////
//Class Config

Config::Config(char fname[]) {
	char dir[505];
	int size = 500, len;
	GetModuleFileName(NULL, dir, size);
	len = strlen(dir);
	while(dir[len-1] != '\\') len--;
	dir[len] = 0;
	strcat(dir, fname);

	strcpy(fname_, dir);
}

Config::~Config() {

}

DWORD Config::read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPTSTR lpReturnedString) {
	if(fname_ == NULL) {
		assert("Config(), file name not init");
	}

	int nSize = 100;
	DWORD hr = GetPrivateProfileString(lpAppName, lpKeyName, NULL, lpReturnedString, nSize, fname_);
	return hr;
}

DWORD Config::read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, int& retval) {
	if(fname_ == NULL) {
		assert("Config(), file name not init");
	}

	int nSize = 100;
	char tmp[100];
	DWORD hr = GetPrivateProfileString(lpAppName, lpKeyName, NULL, tmp, nSize, fname_);

	retval = atoi(tmp);

	return hr;
}

DWORD Config::read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, float& retval) {
	if(fname_ == NULL) {
		assert("Config(), file name not init");
	}

	int nSize = 100;
	char tmp[100];
	DWORD hr = GetPrivateProfileString(lpAppName, lpKeyName, NULL, tmp, nSize, fname_);

	retval = atof(tmp);

	return hr;
}

BOOL Config::write_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPTSTR lpString) {
	if(fname_ == NULL) {
		assert("Config(), file name not init");
	}

	BOOL hr = WritePrivateProfileString(lpAppName, lpKeyName, lpString, fname_);
	return hr;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Class ServerConfig
ServerConfig::ServerConfig(char fname[]): Config(fname) {

}

void ServerConfig::load_config() {
	read_property("command_server", "fps", max_fps_);
	read_property("command_server", "port", command_port_);

	read_property("mesh_decimate", "low_bound", mesh_low_);
	read_property("mesh_decimate", "up_bound", mesh_up_);
	read_property("mesh_decimate", "ratio", mesh_ratio_);

	read_property("mesh_decimate", "ban", ban_ib_);
	
}

void ServerConfig::show_config() {
	printf("[game_server]\n");
	printf("\tfps = %d\n", max_fps_);

	printf("[command_server]\n");
	printf("\tport = %d\n", command_port_);
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Class ClientConfig
ClientConfig::ClientConfig(char fname[]): Config(fname) {

}

void ClientConfig::load_config(int client_num) {
	read_property("command_server", "ip", srv_ip_);

	char p_str[100];
	sprintf(p_str, "port_%d", client_num);

	read_property("command_server", p_str, srv_port_);
}

void ClientConfig::show_config() {
	printf("[command_server]\n");
	printf("\tip = %s\n", srv_ip_);
	printf("\tport = %d\n", srv_port_);
}


/////////////////////////////////////////////////////////////////////
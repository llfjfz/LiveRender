#ifndef __CONFIG__
#define __CONFIG__

#include "utility.h"

class Config {
public:

	Config(char fname[]);
	~Config();

	DWORD read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPTSTR lpReturnedString);
	DWORD read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, int& retval);
	DWORD read_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, float& retval);
	BOOL write_property(LPCTSTR lpAppName, LPCTSTR lpKeyName, LPTSTR lpString);

private:
	char fname_[100];
};

class ServerConfig : public Config {
public:
	ServerConfig(char fname[]);

	void load_config();
	void show_config();

	int command_port_;
	int max_fps_;

	int mesh_low_;
	int mesh_up_;
	float mesh_ratio_;

	int ban_ib_;
};

class ClientConfig : public Config {
public:
	ClientConfig(char fname[]);

	void load_config(int client_num);
	void show_config();

	char srv_ip_[100];
	int srv_port_;
};

#endif

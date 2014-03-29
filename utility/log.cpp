#include "utility.h"
#include "log.h"
#include <time.h>
#include <stdlib.h>

char Log::fname_[100];
ofstream Log::fs_;
int Log::is_init_ = false;

time_t tv;

void Log::init(const char* fname) {
	if(is_init_) return;
	is_init_ = true;

	strcpy(fname_, fname);
	fs_.open(fname_, ios::out);
	log("Log::init(), file name=%s\n", fname_);
}

void Log::close() {
	log("Log::close() called\n");
	fs_.close();
}

void Log::log(const char* text, ...) {
#ifdef ENABLE_LOG
	if(!is_init_) init("fuck.log");
	char buffer[MAX_CHAR];
	char timestr[30];

	tv = time(0);
	tm* ltime = localtime(&tv);
	strftime(timestr, sizeof(timestr), "%H:%M:%S", ltime);

	va_list ap;
	va_start(ap, text);
	vsprintf(buffer, text, ap);
	va_end(ap);

	fs_ << timestr << ": " << buffer;
	fs_.flush();

#endif
}

void Log::slog(const char* text, ...) {
	if(!is_init_) init("sfuck.log");
	char buffer[MAX_CHAR];

	char timestr[30];

	tv = time(0);
	tm* ltime = localtime(&tv);
	strftime(timestr, sizeof(timestr), "%H:%M:%S", ltime);

	va_list ap;
	va_start(ap, text);
	vsprintf(buffer, text, ap);
	va_end(ap);

	fs_ << timestr << ": " << buffer;
	fs_.flush();
}




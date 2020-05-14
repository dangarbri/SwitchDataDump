#pragma once
#include <switch.h>

class AccountAPI
{
public:
	AccountAPI();
	bool IsInitialized();
	s32 GetUserCount();
	void PrintAllUserInfo();
	~AccountAPI();
private:
	bool _is_initialized = false;
};
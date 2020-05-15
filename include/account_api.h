#pragma once
#include <switch.h>

class AccountAPI
{
public:
	AccountAPI();
	bool IsInitialized();
	s32 GetUserCount();
	void PrintAllUserInfo();
	void PrintApplicationInfo();
	~AccountAPI();
private:
	bool _is_initialized = false;
	void PrintUserProfile(AccountUid &uid);
};
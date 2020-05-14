#include "account_api.h"
#include <cstdio>
#include <new>

using namespace std;

AccountAPI::AccountAPI()
{
	Result rc = accountInitialize(AccountServiceType_System);
    if (R_FAILED(rc))
    {
        printf("accountInitialize() failed: 0x%X", rc);
    }
    else
    {
        _is_initialized = true;
    }
}

bool AccountAPI::IsInitialized(void)
{
    return _is_initialized;
}

s32 AccountAPI::GetUserCount()
{
    s32 user_count = -1;
    Result rc = accountGetUserCount(&user_count);
    if (R_FAILED(rc))
    {
        printf("failed to get number of users. code 0x%X\n", rc);
    }

    return user_count;
}

void AccountAPI::PrintAllUserInfo()
{
    s32 user_count = 0;
    s32 users_returned = 0;
    Result rc;
    AccountUid* uid_buffer = nullptr;

    user_count = GetUserCount();
    if (user_count > 0)
    {
        uid_buffer = new(nothrow) AccountUid[2];
        if (uid_buffer == nullptr)
        {
            printf("Failed to allocate memory");
            return;
        }
        else
        {
            rc = accountListAllUsers(uid_buffer, user_count, &users_returned);
            if (R_FAILED(rc))
            {
                printf("Failed to load users. code 0x%X\n", rc);
            }
            else
            {
                for (s32 i = 0; i < user_count; i++)
                {
                    AccountUid* p_uid = &uid_buffer[i];
                    printf("Found User ID: 0x%016lX_%016lX\n", p_uid->uid[0], p_uid->uid[1]);
                }
            }

            delete uid_buffer;
        }
    }
}

AccountAPI::~AccountAPI()
{
    if (_is_initialized)
    {
        accountExit();
    }
}

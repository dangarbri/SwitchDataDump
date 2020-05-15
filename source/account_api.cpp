#include "account_api.h"
#include "error_string.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <new>
#include <vector>

using namespace std;

AccountAPI::AccountAPI()
{
    printf("Initializing account API\n");
	Result rc = accountInitialize(AccountServiceType_System);
    if (R_FAILED(rc))
    {
        printf("Account initialization failed\n");
        ERROR_RESULT_PRINT(rc);
    }
    else
    {
        printf("Account initialization successed\n");
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
    if (IsInitialized())
    {
        Result rc = accountGetUserCount(&user_count);
        if (R_FAILED(rc))
        {
            ERROR_RESULT_PRINT(rc);
        }
    }
    return user_count;
}

void AccountAPI::PrintUserProfile(AccountUid& uid)
{
    AccountProfile user_profile = { 0 };
    AccountProfileBase profile_base = { 0 };
    Result rc;
    rc = accountGetProfile(&user_profile, uid);
    CHECK_RESULT(rc)
    if (R_SUCCEEDED(rc))
    {
        // GetProfile and ProfileGet, quite confusing...
        accountProfileGet(&user_profile, NULL, &profile_base);
        cout << "Found user: " << profile_base.nickname << endl;

        accountProfileClose(&user_profile);
    }
}

void AccountAPI::PrintAllUserInfo()
{
    if (IsInitialized())
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
                    ERROR_RESULT_PRINT(rc);
                }
                else
                {
                    for (s32 i = 0; i < user_count; i++)
                    {
                        //AccountUid* p_uid = &uid_buffer[i];
                        //printf("Found User ID: 0x%016lX_%016lX\n", p_uid->uid[0], p_uid->uid[1]);
                        PrintUserProfile(uid_buffer[i]);
                    }
                }

                delete uid_buffer;
            }
        }
    }
}

void AccountAPI::PrintApplicationInfo()
{
    Result rc;
    NsApplicationControlData control_data;
    u64 return_size;
    NsApplicationRecord record;
    NacpLanguageEntry* p_lang_entry = nullptr;
    s32 offset = 0;
    s32 return_count = 0;
    char name[0x201];
    // Initialize ns before making any api calls
    rc = nsInitialize();
    CHECK_RESULT(rc);
    if (R_SUCCEEDED(rc))
    {
        // I don't see a way of counting application records in the API
        // so just keep requesting records until we don't get anything back
        do {
            // Read one appliction record
            rc = nsListApplicationRecord(&record, 1, offset++, &return_count);
            CHECK_RESULT(rc);
            if (R_SUCCEEDED(rc) && (return_count > 0))
            {
                rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, record.application_id, &control_data, sizeof(control_data), &return_size);
                CHECK_RESULT(rc);
                if (return_size < sizeof(control_data.nacp))
                {
                    cout << "Unexpected number of bytes returned from nsGetApplicationControlData" << endl;
                    break;
                }
            }
            else
            {
                break;
            }

            if (R_SUCCEEDED(rc))
            {
                // Function takes a pointer to a pointer.
                rc = nacpGetLanguageEntry(&control_data.nacp, &p_lang_entry);
                CHECK_RESULT(rc);
            }

            if (R_SUCCEEDED(rc))
            {
                memset(name, 0, sizeof(name));
                // Don't assume lang_entry.name is null terminated
                strncpy(name, p_lang_entry->name, sizeof(name) - 1);
                cout << "Found application: " << name << endl;
                cout << "  app_id 0x" << hex << uppercase << record.application_id << dec << nouppercase << endl;
            }
        } while (return_count > 0);
        // Done with ns, close handle
        nsExit();
    }
}

AccountAPI::~AccountAPI()
{
    if (IsInitialized())
    {
        accountExit();
    }
}

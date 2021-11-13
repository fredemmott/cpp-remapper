#include <filesystem>
#include <memory>
#include <functional>
#include <set>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#pragma warning(push)
#pragma warning(disable: 4005)
#include <ntstatus.h>
#pragma warning(pop)
#include <evntprov.h>
#include <dpfilter.h>

#include <cfgmgr32.h>

#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

#include <initguid.h>
#include <devpkey.h>
#include <devguid.h>
#include <winioctl.h>

#define IDS_CONTROL_DEVICE_NAME L"\\\\.\\HidHide"

#include <combaseapi.h>

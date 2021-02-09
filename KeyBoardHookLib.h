#ifndef _KEYBOARD_EVENT_H_
#define _KEYBOARD_EVENT_H_
#if _MSC_VER > 1000
#pragma once
#endif
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#define LIBSPEC __declspec(dllexport)

   LIBSPEC BOOL InstallKeyBoardHook(HWND hWndParent);
   LIBSPEC BOOL UnInstallKeyBoardHook(HWND hWndParent);
#undef LIBSPEC

   #define UWM_KEYBOARD_MSG ("UWM_KEYBOARD_USER_MSG")
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _KEYBOARD_EVENT_H_
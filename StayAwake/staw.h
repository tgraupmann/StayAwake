/*
 *   Stay Awake - prevents Windows from starting the screen saver.
 *   Copyright (C) 2012  Micha³ Leœniewski
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <Windows.h>

/* Hotkey IDs */
#define STAYAWAKE_HOTKEY_ID      1985

/* Internal messages */
#define WM_STAYAWAKE_NEWINSTANCE  WM_USER + 0x01

/* Window class and name */
#define STAYAWAKE_WIN_CLASS  TEXT("StayAwakeMessageWindowClass")
#define STAYAWAKE_WIN_NAME   TEXT("StayAwakeMessageWindow")

/* Settings structure -- all these settings are saved in the registry */
typedef struct _SettingsT {
    BOOL hotkeyEnabled;
    UINT hotkey;
    BOOL balloons;
    BOOL balloonsStartup;
    BOOL remember;
    BOOL safemode;
    BOOL state;
} SettingsT;

extern SettingsT settings;

extern HWND hMainWindow;
extern HINSTANCE hInstance;

/* error.c */
void fatal_error(_TCHAR *message);
void warning(_TCHAR *message);
void checkRegOperation(LSTATUS status, _TCHAR *message);
void checkPtr(void *ptr, _TCHAR *message);

/* about.c */
void openAboutDlg();

/* settingsdlg.c */
void openSettingsDlg();

/* hotkey.c */
void enableHotkeys();
void disableHotkeys();

/* settings.c */
void loadSettings();
void saveSettings();

/* autostart.c */
void autostartEnable(BOOLEAN enable);
BOOLEAN autostartState();
void autostartUpdate();

/* timer.c */
void enableStayAwake(BOOL state);

/* mainwin.c */
void createMainWinow();

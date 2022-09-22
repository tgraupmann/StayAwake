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
#include "common.h"

static const TCHAR regKey[] = TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run");
static const TCHAR appTitle[] = TEXT("StayAwake");

void autostartEnable(BOOLEAN enable) {
    HKEY key;
    
    if (RegOpenKey(HKEY_CURRENT_USER, regKey, &key) != ERROR_SUCCESS) {
        return;
    }
    
    if (enable) {
        /* Get the path to the current executable. Normally this can be
           achieved by simply calling GetModuleFileName. Unfortunately,
           we can never know how the result path really is, so we call
           the function in a loop increasing the buffer size by 128
           bytes until the result path fits. */
        DWORD bufsize = 128;
        DWORD pathsize = 0;
        TCHAR *executablepath;
        while (TRUE) {
            executablepath = (TCHAR *) malloc(bufsize * sizeof(TCHAR));
            pathsize = GetModuleFileName(NULL, executablepath, bufsize);
            if (pathsize < bufsize)
                break;
            free(executablepath);
            bufsize += 128;
        }
        
        /* Write the value to the registry. */
        checkRegOperation(RegSetValueEx(key, appTitle, 0, REG_SZ,
                                        (LPBYTE) executablepath,
                                        (pathsize + 1) * sizeof(TCHAR)),
                          TEXT("Couldn't enable application autostart."));
                          
        free(executablepath);
    } else {
        /* If we're disabling autostart, we just try to delete the
           value with the given name. An error here would mean,
           that there is no such value, so we can ignore it. */
        RegDeleteValue(key, appTitle);
    }
    
    checkRegOperation(
        RegCloseKey(key),
        TEXT("Error closing registry key. Now this is really weird.")
    );
}

BOOLEAN autostartState() {
    HKEY key;
    DWORD size = 0;
    
    if (RegOpenKey(HKEY_CURRENT_USER, regKey, &key) != ERROR_SUCCESS) {
        /* Assume autostart is disabled if an error occurs. Actually
           an error here would be really weird. */
        return FALSE;
    }
    
    /* When checking state, we just check if the given value exists
       in the registry and that its data is not empty. */
    RegQueryValueEx(key, appTitle, NULL, NULL, NULL, &size);
    
    checkRegOperation(
        RegCloseKey(key),
        TEXT("Error closing registry key. Now this is really weird.")
    );
    
    return size != 0;
}

void autostartUpdate() {
    autostartEnable(autostartState());
}
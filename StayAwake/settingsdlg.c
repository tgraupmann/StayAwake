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

static BOOLEAN dialogVisible = FALSE;

static INT_PTR CALLBACK DlgProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_INITDIALOG:
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY), BM_SETCHECK, settings.hotkeyEnabled, 0);
            EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY), settings.hotkeyEnabled);
            SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY), HKM_SETHOTKEY, settings.hotkey, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), BM_SETCHECK, settings.balloons, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_STARTUP_BALLOON), BM_SETCHECK, settings.balloonsStartup, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_REMEMBER), BM_SETCHECK, settings.remember, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_ALTERNATIVE), BM_SETCHECK, settings.safemode, 0);
            SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), BM_SETCHECK, autostartState(), 0);
            dialogVisible = TRUE;
            disableHotkeys();
            return TRUE;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_CHECKBOX_HOTKEY: {
                        BOOL state = BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY), BM_GETCHECK, 0, 0);
                        EnableWindow(GetDlgItem(hwndDlg, IDC_HOTKEY), state);
                        return TRUE;
                    }
                case IDC_CHECKBOX_ALTERNATIVE: {
                        if (BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_ALTERNATIVE), BM_GETCHECK, 0, 0)) {
                            MessageBox(hwndDlg,
                                       TEXT("In advanced mode Stay Awake moves the mouse cursor back and forth by a few pixels every second.")
                                       TEXT("Use this mode only if you have trouble with the normal mode."),
                                       TEXT("Warning"), MB_ICONINFORMATION | MB_OK);
                        }
                        return TRUE;
                    }
                case IDOK:
                    settings.hotkeyEnabled = SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_HOTKEY), BM_GETCHECK, 0, 0) != 0;
                    settings.hotkey = (UINT) SendMessage(GetDlgItem(hwndDlg, IDC_HOTKEY), HKM_GETHOTKEY, 0, 0);
                    settings.balloons =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_BALLOONS), BM_GETCHECK, 0, 0);
                    settings.balloonsStartup =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_STARTUP_BALLOON), BM_GETCHECK, 0, 0);
                    settings.remember =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_REMEMBER), BM_GETCHECK, 0, 0);
                    settings.safemode =
                        BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_ALTERNATIVE), BM_GETCHECK, 0, 0);
                    autostartEnable(BST_CHECKED == SendMessage(GetDlgItem(hwndDlg, IDC_CHECKBOX_AUTOSTART), BM_GETCHECK, 0, 0));
                    saveSettings();
                case IDCANCEL:
                    EndDialog(hwndDlg, LOWORD(wParam));
                    return TRUE;
            }
            return FALSE;
        case WM_DESTROY:
            dialogVisible = FALSE;
            enableHotkeys();
            return TRUE;
        default:
            return FALSE;
    }
    return TRUE;
}

void openSettingsDlg() {
    if (!dialogVisible)
        DialogBox(hInstance,
                  MAKEINTRESOURCE(IDD_SETTINGS),
                  hMainWindow,
                  DlgProc);
}
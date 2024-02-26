#include "CodingThreadDlg.h"

CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;

CodingThreadDlg::CodingThreadDlg(void)
{
	ptr = this;
}

CodingThreadDlg::~CodingThreadDlg(void)
{
	
}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
	ReleaseMutex(hMutex); // освобождения мьютекса
	EndDialog(hwnd, 0);

}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
	CodingThreadDlg *ptr = (CodingThreadDlg *)lp;
	wstring line;
	wifstream in(TEXT("music.txt"),ios::in);
	if(!in)
	{
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));

	DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

		wofstream out("coding.txt", ios::out | ios::trunc);
		const int KEY = 100;
		while(!in.eof())
		{
			getline(in, line);
			out << line << endl;
		}
		out.close();
		in.close();
		ReleaseMutex(hMutex); // освобождения мьютекса

		MessageBox(ptr->hDialog, TEXT("Запись данных в файл coding.txt завершена!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);


	return 0;
}
DWORD WINAPI OutThread(LPVOID lp)
{
	CodingThreadDlg* ptr = (CodingThreadDlg*)lp;
	wstring line;
	wifstream in(TEXT("coding.txt"),ios::in);
	if (!in)
	{
		MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
		return 1;
	}
	HANDLE hMutexOut = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));
	DWORD dwAnswer = WaitForSingleObject(hMutexOut, INFINITE);
	while (!in.eof())
	{
		getline(in,line);
		SendMessageW(HWND(lp), LB_ADDSTRING, 0, LPARAM(line.c_str()));
	}
	in.close();
	ReleaseMutex(hMutexOut); // освобождения мьютекса
	return 0;
}

BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	hDialog = hwnd;
	hList = GetDlgItem(hwnd, IDC_LIST1);
	CreateThread(NULL, 0, Coding_Thread, this, 0, NULL); 
	CreateThread(NULL, 0, OutThread, hList, 0, NULL); 
	return TRUE;
}


BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
	}
	return FALSE;
}
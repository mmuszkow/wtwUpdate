#include "stdinc.h"
#include "Updater/CheckThread.h"

WTWPLUGINFO plugInfo = {
	sizeof(WTWPLUGINFO),						// rozmiar struktury
	L"wtwUpdate",								// nazwa wtyczki
	L"Aktualizacje dodatków",					// opis wtyczki
	L"© 2010-2014 Maciej Muszkowski",			// prawa autorskie
	L"Maciej Muszkowski",						// autor
	L"maciek.muszkowski@gmail.com",				// dane do kontaktu z autorem
	L"http://www.alset.pl/Maciek",				// strona www autora
	L"",										// url do pliku xml z danymi do autoupdate
	PLUGIN_API_VERSION,							// wersja api
	MAKE_QWORD(0, 0, 0, 1),						// wersja wtyczki
	WTW_CLASS_UTILITY,							// klasa wtyczki
	NULL,										// f-cja wywolana podczas klikniecia "o wtyczce"
	L"{7AE5B430-CF7E-11E3-9C1A-0800200C9A66}",	// guid (jezeli chcemy tylko jedna instancje wtyczki)
	NULL,										// dependencies
	0, 0, 0, 0									// zarezerwowane (4 pola)
};

HINSTANCE hInst = NULL;

extern "C" {

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	hInst = hinstDLL;
	return 1;
}

WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) {
    return &plugInfo;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fn) {
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	wtwUpdate::updater::FilePath::initDirPaths(fn);

	wtwUpdate::updater::CheckThread tt;
	//HWND hMain;
	//fn->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);
	//wtwUpdate::ui::UpdateWnd wnd(hInst, hMain);

    return 0;
}

int __stdcall pluginUnload(DWORD callReason) {
	return 0;
}

} // extern "C"

#include "stdinc.h"
#include "UI/RichEdit/RichEdit.hpp"
#include "UI/UpdateWnd.hpp"
#include "UI/WtwMenuItem.hpp"
#include "Updater/InstallThread.hpp"
#include "Updater/UpdateThread.hpp"
#include "Updater/ThreadScheduler.hpp"
#include "Updater/FilePath.hpp"

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

using namespace wtwUpdate::ui;
using namespace wtwUpdate::updater;

extern "C" {

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
	hInst = hinstDLL;
	return 1;
}

WTWPLUGINFO* __stdcall queryPlugInfo(DWORD apiVersion, DWORD masterVersion) {
    return &plugInfo;
}

class UpdateWndMenuItem : public WtwMenuItem {
	static WTW_PTR wtwMenuClickFunc(WTW_PARAM, WTW_PARAM, void* cData) {
		wtw::CJson* json = UpdateThread::get().downloadJson(L"http://muh.cba.pl/central.json");
		UpdateWnd::get().open(hMain, hInst, json);
		if (json)
			wtw::CJson::decref(json);
		return 0;
	}
public:
	UpdateWndMenuItem() : WtwMenuItem(WTW_MENU_ID_MAIN_OPT, L"wtwUpdate/updateWnd", L"Aktualizuj", wtwMenuClickFunc) {
	}
};

UpdateWndMenuItem* menuItem;

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* _fn) {
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	fn = _fn;
	FilePath::initDirPaths();
	MyRichEdit::RichEdit::libInit();

	
	//UpdateThread& updateThread = UpdateThread::get();
	//ThreadScheduler& scheduler = ThreadScheduler::get();
	//scheduler.schedule(updateThread, 300000, true); // 5 min after start
	//scheduler.schedule(updateThread, 86400000, false); // once every 24h

	fn->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);
	menuItem = new UpdateWndMenuItem();

    return 0;
}

int __stdcall pluginUnload(DWORD callReason) {
	delete menuItem;

	UpdateWnd::get().destroy();
	ThreadScheduler::get().destroyAll();
	UpdateThread::get().abort();
	InstallThread::get().abort();

	MyRichEdit::RichEdit::libDeinit();
	fn = NULL;

	return 0;
}

} // extern "C"

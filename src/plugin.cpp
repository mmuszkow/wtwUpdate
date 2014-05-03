#include "stdinc.h"
#include "UI/UpdateWnd.h"
#include "UI/RichEdit/RichEdit.h"
#include "Updater/UpdateThread.h"
#include "Updater/ThreadScheduler.h"

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
HWND hMain = NULL;

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

WTW_PTR wtwMenuClickFunc(WTW_PARAM, WTW_PARAM, void* cData) {
	wtw::CJson* json = UpdateThread::get().downloadJson(L"http://muh.cba.pl/central.json");
	wtwUpdate::ui::UpdateWnd::get().open(hMain, hInst, json);
	return 0;
}

int __stdcall pluginLoad(DWORD callReason, WTWFUNCTIONS* fn) {
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	FilePath::initDirPaths(fn);
	MyRichEdit::RichEdit::libInit();

	
	UpdateThread& updateThread = UpdateThread::get();
	updateThread.setFn(fn);
	InstallThread::get().setFn(fn);

	ThreadScheduler& scheduler = ThreadScheduler::get();
	scheduler.setWtwFn(fn);
	scheduler.schedule(updateThread, 300000, true); // 5 min after start
	scheduler.schedule(updateThread, 86400000, false); // once every 24h

	fn->fnCall(WTW_GET_MAIN_HWND_EX, reinterpret_cast<WTW_PARAM>(&hMain), NULL);

	wtwMenuItemDef menuDef;
	menuDef.menuID = WTW_MENU_ID_MAIN_OPT;
	menuDef.callback = wtwMenuClickFunc;
	menuDef.itemId = L"wtwUpdate/updateWnd";
	menuDef.menuCaption = L"Aktualizuj";
	fn->fnCall(WTW_MENU_ITEM_ADD, menuDef, NULL);

    return 0;
}

int __stdcall pluginUnload(DWORD callReason) {
	ThreadScheduler::get().destroyAll();

	//wtwMenuItemDef menuDef;
	//menuDef.menuID = WTW_MENU_ID_MAIN_OPT;
	//menuDef.itemId = L"wtwUpdate/updateWnd";
	//fn->fnCall(WTW_MENU_ITEM_ADD, menuDef, NULL);

	MyRichEdit::RichEdit::libDeinit();

	return 0;
}

} // extern "C"

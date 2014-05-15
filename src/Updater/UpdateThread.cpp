#pragma once

#include "stdinc.h"
#include "UpdateThread.hpp"
#include "UI/UpdateWnd.hpp"
#include "JsonObjs/AddonsList.hpp"
#include "Utils/Serialization.hpp"

namespace wtwUpdate {
	namespace updater {
		WTW_PTR UpdateThread::onNotificationClick(WTW_PARAM wPar, WTW_PARAM lPar, void* cData) {
			wtwTrayNotifyEvent* ev = reinterpret_cast<wtwTrayNotifyEvent*>(wPar);
			if (ev->event == WTW_TN_EVENT_LCLICKED)
				ui::UpdateWnd::get().start();
			return 0;
		}

		DWORD UpdateThread::proc(LPVOID args) {
			UpdateThread* thread = static_cast<UpdateThread*>(args);
			thread->setRunning(true);

			// download JSON
			//wtw::CJson* root = utils::downloadJson(L"http://wtw-addons.cba.pl/central.json");
			bds_strings_map* map = bdf_strings_map_create(8192);
			bds_node* root = utils::downloadBson(L"http://wtw-addons.cba.pl/central.bson", map);
			if (!root) {
				// logged in downloadBson
				bdf_strings_map_destroy(map);
				thread->setRunning(false);
				return 1;
			}

			// get all addons and check which need to be updated
			// TODO: this should be also filtered by installation request, only the addons installed by wtwUpdate should be here
			json::AddonsList addons(root);
			unsigned int needUpdate = 0, i, len = addons.size();
			for (i = 0; i < len; i++) {
				if (thread->isAborted()) {
					//wtw::CJson::decref(root);
					bdf_node_destroy(root, false);
					bdf_strings_map_destroy(map);
					thread->setRunning(false);
					return 0;
				}
				if (addons[i].getState() == json::Addon::MODIFIED)
					needUpdate++;
			}

			//wtw::CJson::decref(root);
			bdf_node_destroy(root, false);
			bdf_strings_map_destroy(map);

			if (needUpdate == 0) {
				thread->setRunning(false);
				return 0;
			}

			if (needUpdate == 1)
				notifyEx(0, onNotificationClick, NULL, L"Dostępna aktualizacja 1 dodatku");
			else
				notifyEx(0, onNotificationClick, NULL, L"Dostępne aktualizacje %d dodatków", needUpdate);

			thread->setRunning(false);
			return 0;
		}
	}
}

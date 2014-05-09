#include "stdinc.h"
#include "Bitmap.hpp"

namespace wtwUpdate {
	namespace ui {
        Bitmap::Bitmap() : _hBmp(NULL), _wtwManaged(false) { }

        Bitmap::Bitmap(const wchar_t* id, const wchar_t* themeFn, int resId) {
            wtwGraphics graph;
            graph.hInst = hInst;
            graph.graphId = id;
            graph.filePath = themeFn;
            graph.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
            if (fn->fnCall(WTW_GRAPH_LOAD, graph, 0) == TRUE) {
                graph.filePath = NULL;
                graph.flags = WTW_GRAPH_FLAG_GENERATE_HBITMAP;
                _hBmp = reinterpret_cast<HBITMAP>(fn->fnCall(WTW_GRAPH_GET_IMAGE, graph, NULL));
                _wtwManaged = true;
            } else {
                _hBmp = LoadBitmap(hInst, MAKEINTRESOURCE(resId));
                _wtwManaged = false;
            }
        }

        void Bitmap::release() {
            // if the HBITMAP was obtained from WTW, it will take care of deleting it
            if (_hBmp && !_wtwManaged)
                DeleteObject(_hBmp);
        }
    }
}

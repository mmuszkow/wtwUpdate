#pragma once

#include "Addon.hpp"
#include "Utils/Settings.hpp"

namespace wtwUpdate {
	namespace json {
		/// List of all addons in JSON file. Similar to AddonsTree but flat.
		/// This class uses values stored in setting and needs valid WTWFUNCTIONS* pointer.
		class AddonsList : public std::vector<json::Addon> {
			void recAddon(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s);
			void recSection(wtw::CJson* json, const std::string& dir, const wtwUtils::Settings& s);
			void recAddon(bds_node* bson, const std::string& dir, const wtwUtils::Settings& s);
			void recSection(bds_node* bson, const std::string& dir, const wtwUtils::Settings& s);
		public:
			AddonsList() { }			
			AddonsList(wtw::CJson* json);
			AddonsList(bds_node* bson);
			AddonsList(const std::vector<json::Addon>& list) : std::vector<json::Addon>(list) {	}

			std::vector<json::Addon> removeConflicted();
		};
	}
}

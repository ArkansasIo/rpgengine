#pragma once

#include <string>
#include <unordered_map>

namespace arclight {

class OOScriptClass {
public:
	explicit OOScriptClass(std::string className)
		: name(std::move(className)) {}

	void SetField(const std::string& key, const std::string& value) {
		fields[key] = value;
	}

	std::string GetField(const std::string& key) const {
		auto it = fields.find(key);
		if (it == fields.end()) {
			return {};
		}
		return it->second;
	}

	const std::string& GetName() const {
		return name;
	}

private:
	std::string name;
	std::unordered_map<std::string, std::string> fields;
};

} // namespace arclight

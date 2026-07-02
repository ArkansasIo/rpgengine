#ifndef ARCLIGHT_IENGINE_PLUGIN_H
#define ARCLIGHT_IENGINE_PLUGIN_H

#include <string>

namespace arclight {

class IEnginePlugin {
public:
	virtual ~IEnginePlugin() = default;

	virtual const char* GetName() const = 0;
	virtual bool OnLoad() = 0;
	virtual void OnUnload() = 0;
};

} // namespace arclight

#endif // ARCLIGHT_IENGINE_PLUGIN_H

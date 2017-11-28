#pragma once

#include <ModuleConfiguration.hpp>
#include <core/mw/CoreModule.hpp>

namespace core {
namespace hw {
class Pad;
}
}

class Module:
    public core::mw::CoreModule
{
public:
// --- DEVICES ----------------------------------------------------------------

// ----------------------------------------------------------------------------

    static bool
    initialize();


    Module();
    virtual ~Module() {}
};

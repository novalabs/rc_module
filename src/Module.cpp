/* COPYRIGHT (c) 2016-2017 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#include <core/snippets/CortexMxFaultHandlers.h>

#include <core/mw/Middleware.hpp>
#include <core/mw/transport/RTCANTransport.hpp>

#include <core/hw/GPIO.hpp>
#include <core/hw/IWDG.hpp>
#include <core/os/Thread.hpp>

#include <Module.hpp>

// LED
using LED_PAD = core::hw::Pad_<core::hw::GPIO_C, LED_PIN>;
static LED_PAD _led;

// MODULE DEVICES

// SYSTEM STUFF
static core::os::Thread::Stack<1024> management_thread_stack;
static core::mw::RTCANTransport      rtcantra(&RTCAND1);

RTCANConfig rtcan_config = {
    1000000, 100, 60
};


Module::Module()
{}

bool
Module::initialize()
{
#ifdef _DEBUG
    FAULT_HANDLERS_ENABLE(true);
#else
    FAULT_HANDLERS_ENABLE(false);
#endif

    static bool initialized = false;

    if (!initialized) {
        core::mw::CoreModule::initialize();

        core::mw::Middleware::instance().initialize(name(), management_thread_stack, management_thread_stack.size(), core::os::Thread::LOWEST);
        rtcantra.initialize(rtcan_config, canID());
        core::mw::Middleware::instance().start();

        initialized = true;
    }

    return initialized;
} // Board::initialize

// ----------------------------------------------------------------------------
// CoreModule STM32FlashConfigurationStorage
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleSTM32FlashConfigurationStorage.hpp>
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// CoreModule HW specific implementation
// ----------------------------------------------------------------------------
#include <core/snippets/CoreModuleHWSpecificImplementation.hpp>
// ----------------------------------------------------------------------------

/* RC Module template file
 *
 */

#include <ModuleConfiguration.hpp>
#include <Module.hpp>

// --- BOARD IMPL -------------------------------------------------------------
#include <core/hw/GPIO.hpp>
#include <core/hw/PWM.hpp>

// --- MODULE -----------------------------------------------------------------
Module module;

// *** DO NOT MOVE THE CODE ABOVE THIS COMMENT *** //

// --- MESSAGES ---------------------------------------------------------------
#include <core/common_msgs/Led.hpp>
//#include <core/common_msgs/Array12_u16.hpp>

// --- NODES ------------------------------------------------------------------
#include <core/led/Subscriber.hpp>
#include <core/rc/Servo.hpp>

// --- TYPES ------------------------------------------------------------------

// --- CONFIGURATIONS ---------------------------------------------------------
core::led::SubscriberConfiguration led_subscriber_configuration_default;
core::rc::ServoConfiguration servo_configuration_default;

// --- NODES ------------------------------------------------------------------
core::led::Subscriber led_subscriber("led_sub", core::os::Thread::PriorityEnum::LOWEST);
core::rc::Servo       servo("servo", core::os::Thread::PriorityEnum::NORMAL);

// --- MAIN -------------------------------------------------------------------
extern "C" {
    int
    main()
    {
        module.initialize();

        // Device configurations

        // Default configuration
        led_subscriber_configuration_default.topic = "led";

        servo_configuration_default.topic = "servo";
        servo_configuration_default.failsafe = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

        // Add configurable objects to the configuration manager...
        module.configurations().add(led_subscriber, led_subscriber_configuration_default);
        module.configurations().add(servo, servo_configuration_default);

        // ... and load the configuration
        module.configurations().loadFrom(module.configurationStorage());

        // Add nodes to the node manager...
        module.nodes().add(led_subscriber);
        module.nodes().add(servo);

        // ... and let's play!
        module.nodes().setup();
        module.nodes().run();

        // Is everything going well?
        for (;;) {
            if (!module.nodes().areOk()) {
                module.halt("This must not happen!");
            }

            core::os::Thread::sleep(core::os::Time::ms(500));

            // Remember to feed the (watch)dog!
            module.keepAlive();
        }

        return core::os::Thread::OK;
    } // main
}

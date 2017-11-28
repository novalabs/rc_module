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

// --- DEVICE CONFIGURATION ---------------------------------------------------
#if 0
/*
* PWM configuration.
*/
uint16_t in_values[12] = {0};
uint16_t out_values[12] = {0};
static const uint16_t failsafe_values[12] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};

bool failsafe = true;
int idx = 0;
#endif

#if 0
static void icuwidthcb(ICUDriver *icup) {
	core::os::SysLock::ISRScope lock;

	const icucnt_t width = icuGetWidth(icup);


	if ((width > 2500) || (width < 500)) {
		// sync pulse or glitch
		idx = 0;
		return;
	}

	in_values[idx++] = width;

	if (idx >= 8) {
		idx = 0;
	}
}

static void icuperiodcb(ICUDriver *icup) {

}

static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  1000000,                                  /* 1 MHz ICU clock frequency.   */
  icuwidthcb,
  icuperiodcb,
  NULL,
  ICU_CHANNEL_2,
  0
};

/*
 * RC input node.
 */
struct rcin_node_conf {
	const char * name;
	const char * topic;
};

msg_t rcin_node(void * arg) {
	rcin_node_conf * conf = (rcin_node_conf *) arg;
	core::mw::Node node(conf->name);
	core::mw::Publisher<core::common_msgs::Array12_u16> servo_pub;
	core::common_msgs::Array12_u16 * msgp;

	(void) arg;
	chRegSetThreadName(conf->name);
	node.advertise(servo_pub, conf->topic);

	icuStart(&ICUD4, &icucfg);
	icuEnable(&ICUD4);

	while (!chThdShouldTerminate()) {
		if (servo_pub.alloc(msgp)) {
			for (int i = 0; i < 8; i++) {
				msgp->pulse[i] = in_values[i];
			}

			servo_pub.publish(msgp);
		}

		chThdSleepMilliseconds(20);
	}

	return CH_SUCCESS;
}
#endif

#if 0
/*
* PWM cyclic callback.
*/
static void pwmcb(PWMDriver *pwmp) {
	const uint16_t * values;

	(void) pwmp;
	core::os::SysLock::ISRScope lock;

	if (failsafe) {
		values = failsafe_values;
	} else {
		values = out_values;
	}

	if (pwmp == &PWMD1) {
		pwm_lld_enable_channel(pwmp, 0, values[6]);
		pwm_lld_enable_channel(pwmp, 1, values[7]);
		pwm_lld_enable_channel(pwmp, 2, values[5]);
		pwm_lld_enable_channel(pwmp, 3, values[4]);
	}

	if (pwmp == &PWMD2) {
		pwm_lld_enable_channel(pwmp, 0, values[8]);
		pwm_lld_enable_channel(pwmp, 1, values[9]);
		pwm_lld_enable_channel(pwmp, 2, values[10]);
		pwm_lld_enable_channel(pwmp, 3, values[11]);
	}

	if (pwmp == &PWMD3) {
		pwm_lld_enable_channel(pwmp, 0, values[0]);
		pwm_lld_enable_channel(pwmp, 1, values[1]);
		pwm_lld_enable_channel(pwmp, 2, values[2]);
		pwm_lld_enable_channel(pwmp, 3, values[3]);
	}
}

static PWMConfig pwmcfg = {
		1000000, /* 1 MHz PWM clock frequency.   */
		20000, /* 50 Hz frequency. */
		pwmcb,
		{{ PWM_OUTPUT_ACTIVE_HIGH, NULL },
		 { PWM_OUTPUT_ACTIVE_HIGH, NULL },
		 { PWM_OUTPUT_ACTIVE_HIGH, NULL },
		 { PWM_OUTPUT_ACTIVE_HIGH, NULL } },
		 0, 0
};

bool servo_cb(const core::common_msgs::Array12_u16 &msg, void* context) {
	for (int i = 0; i < 8; i++) {
		uint16_t p = msg.values[i];

		if((p >= 500) && (p <= 2500)) {
			out_values[i] = p;
		}
	}

	return true;
}

void rcout_node(void * arg) {
	core::mw::Node node("rc");
	core::mw::Subscriber<core::common_msgs::Array12_u16, 5> servo_sub(servo_cb);

	(void) arg;

	node.subscribe(servo_sub, "rc");

	pwmStart(&PWMD1, &pwmcfg);
	pwmStart(&PWMD2, &pwmcfg);
	pwmStart(&PWMD3, &pwmcfg);

	while (!chThdShouldTerminateX()) {
		failsafe = !node.spin(core::os::Time::ms(1000));
	}
}
#endif


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

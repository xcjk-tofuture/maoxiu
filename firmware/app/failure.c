extern void platform_emergency_stop(void);
void app_fatal(void) {platform_emergency_stop();for(;;){}}

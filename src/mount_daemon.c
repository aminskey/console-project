#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <systemd/sd-device.h>
#include <systemd/sd-event.h>

const char *mount_point = "/home/mingo/game_mount";

static int device_handler(sd_device_monitor *m, sd_device *d, void *ud){}

int main(void){
    __attribute__((cleanup(sd_device_monitor_unrefp))) sd_device_monitor *m = NULL;

    // Initialising monitor
    if(sd_device_monitor_new(&m) == -1){
        perror("Cannot init dev monitor ... :(");
        return -1;
    }

    // Applying appropriate filters
    if(sd_device_monitor_filter_add_match_subsystem_devtype(m, "block", "partition") == -1){
        perror("Cannot apply appropriate filters .. :(");
        return -1;
    }

    // Starting monitor
    if(sd_device_monitor_start(m, device_handler, NULL) == -1){
        perror("Cannot launch device handler");
        return -1;
    }

    // polling events
    if(sd_event_loop(sd_device_monitor_get_event(m)) < 0){
        perror("Cannot run event loop :(");
        return -1;
    }

    return 0;
}

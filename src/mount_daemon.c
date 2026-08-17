#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <systemd/sd-device.h>
#include <systemd/sd-event.h>


const char *mount_point = "/home/mingo/game_mount";
const char *procname;

int mount_stub(const char *src, const char *target, const char *fs, unsigned long f, const void *data) {
    printf("Mounting %s (%s) onto %s\n", src, fs, target);
    return 1;
}

int sendmsg_stub(const char *s) {
    printf("Sending TCP Message: %s\n", s);
    return 1;
}

int getblkInfo(sd_device *d, const char **src, const char **label, const char **fstype) {
    return !(sd_device_get_devname(d, src) 
        & sd_device_get_property_value(d, "ID_FS_LABEL_ENC", label)
        & sd_device_get_property_value(d, "ID_FS_TYPE", fstype));
}

static int device_handler(sd_device_monitor *m, sd_device *d, void *ud){
    sd_device_action_t t;
    char *src, *label, *fstype;


    if(sd_device_get_action(d, &t) != 0){
        fprintf(stderr, "[ %s ]: Cannot Process event\n", procname);
        return -1;
    }

    // This happens when the device is inserted
    if(!t) {
        // If we can't get the relevant info, then report error
        if(!getblkInfo(d, (const char **)&src, (const char **)&label, (const char **)&fstype)) {
            fprintf(stderr, "[ %s ]: Cannot Get Device Info\n", procname);
            return -1;
        }

        // Pause the UI
        sendmsg_stub("pause");

        // Try to mount the storage unit
        return mount_stub(src, mount_point, fstype, 0, NULL);
    }

    // We let the kernel handle the unmounting
    if(t == SD_DEVICE_REMOVE) {
        printf("Forcefully unmounting device\n");
        return sendmsg_stub("play");
    }

    return 1;
}

int main(int argc, const char **argv){
    __attribute__((cleanup(sd_device_monitor_unrefp))) sd_device_monitor *m = NULL;

    procname = argv[0];

    // Initialising monitor
    if(sd_device_monitor_new(&m) == -1){
        perror("Cannot init dev monitor ... :(");
        return -1;
    }

    // Applying appropriate filters
    if(sd_device_monitor_filter_add_match_subsystem_devtype(m, "block", "disk") == -1){
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

#include "cvideo.h"
#include "cvideo.pio.h"

static inline void cvdata_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint data_pin) {
    pio_sm_config c = cvdata_program_get_default_config(offset);

    // Map the state machine's OUT and SIDE pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_set_pins(&c, data_pin, 1);
    sm_config_set_sideset_pins(&c, data_pin);

    sm_config_set_clkdiv(&c, clockdiv);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, data_pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, data_pin, 1, true);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

static inline void cvsync_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint sync_pin) {
    pio_sm_config c = cvsync_program_get_default_config(offset);

    // sm_config_set_sideset(pio, &c, 1, true, false);

    // Map the state machine's OUT and SIDE pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_set_pins(&c, sync_pin, 1);
    sm_config_set_sideset_pins(&c, sync_pin);

    // Set the clock speed
    sm_config_set_clkdiv(&c, clockdiv);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, sync_pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, sync_pin, 1, true);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Tell the state machine the number of video lines per frame (minus 1)
    pio_sm_put(pio, sm, 287);

    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

void cvideo_init(PIO pio, uint data_pin, uint sync_pin) {
    uint h_bits = 64;
    float data_clockdiv =  125 / (h_bits / 52.0);

    uint offset_sync = pio_add_program(pio, &cvsync_program);
    uint offset_data = pio_add_program(pio, &cvdata_program);
    cvdata_program_init(pio, 0, offset_data, data_clockdiv, data_pin);
    cvsync_program_init(pio, 1, offset_sync, 250, sync_pin);
}
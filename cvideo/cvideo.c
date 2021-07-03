#include "cvideo.h"
#include "cvideo.pio.h"
#include "hardware/clocks.h"

// Sync PIO needs 2us per instruction
#define SYNC_CLOCK_HZ 500000 
// Data transmits for 52us
#define DATA_INTERVAL 0.000052

static inline void cvsync_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint sync_pin);
static inline void cvdata_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint data_pin); 

void cvideo_init(PIO pio, uint data_pin, uint sync_pin, uint bits_per_line) {
    // Run the dataclock 32x faster than needed to reduce horizontal jitter due to synchronisation between SMs
    float data_clockdiv = (clock_get_hz(clk_sys) / (bits_per_line / DATA_INTERVAL)) / 32; 
    float sync_clockdiv = clock_get_hz(clk_sys) / SYNC_CLOCK_HZ;

    uint offset_sync = pio_add_program(pio, &cvsync_program);
    uint offset_data = pio_add_program(pio, &cvdata_program);

    cvdata_program_init(pio, 0, offset_data, data_clockdiv, data_pin);
    cvsync_program_init(pio, 1, offset_sync, sync_clockdiv, sync_pin);
}

static inline void cvdata_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint data_pin) {
    pio_sm_config c = cvdata_program_get_default_config(offset);

    // Map the state machine's OUT and SIDE pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_out_pins(&c, data_pin, 1);
    sm_config_set_sideset_pins(&c, data_pin);

    sm_config_set_clkdiv(&c, clockdiv);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, data_pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, data_pin, 1, true);

    // Enable autopull and set threshold to 32 bits
    c.shiftctrl = (1u<<PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) |
                  (0u<<PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);

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

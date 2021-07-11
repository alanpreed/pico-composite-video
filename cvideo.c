#include "cvideo.h"
#include "cvideo.pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include <stdio.h>

// Sync PIO needs 2us per instruction
#define SYNC_INTERVAL 0.000002
// Data transmits for 52us
#define DATA_INTERVAL 0.000052

#define DATA_SM_ID 0
#define SYNC_SM_ID 1

static PIO cvideo_pio;
static uint pio_irq_id = PIO0_IRQ_1;
static cvideo_data_callback_t data_callback;

static inline void cvsync_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint sync_pin);
static inline void cvdata_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint data_pin); 

static void cvdata_isr(void) {
    pio_sm_put(cvideo_pio, DATA_SM_ID, data_callback());
    irq_clear(pio_irq_id);
}

void cvideo_init(PIO pio, uint data_pin, uint sync_pin, cvideo_data_callback_t callback) {
    cvideo_pio = pio;
    data_callback = callback;
    if (pio_get_index(pio) == 1){
        pio_irq_id = PIO1_IRQ_1;
    }

    // Run the data clock 32x faster than needed to reduce horizontal jitter due to synchronisation between SMs
    float data_clockdiv = (clock_get_hz(clk_sys) / (CVIDEO_PIX_PER_LINE / DATA_INTERVAL)) / 2;// /  32;
    float sync_clockdiv = clock_get_hz(clk_sys) * SYNC_INTERVAL;// / SYNC_CLOCK_HZ;

    printf("Data clockdiv %f\r\n", data_clockdiv);
    printf("Sync clockdiv %f\r\n", sync_clockdiv);
    
    uint offset_sync = pio_add_program(pio, &cvsync_program);
    uint offset_data = pio_add_program(pio, &cvdata_program);

    cvdata_program_init(pio, DATA_SM_ID, offset_data, data_clockdiv, data_pin);
    cvsync_program_init(pio, SYNC_SM_ID, offset_sync, sync_clockdiv, sync_pin);

    // Enable FIFO refill interrupt for data state machine
    irq_set_enabled(pio_irq_id, true);
    irq_set_exclusive_handler(pio_irq_id, cvdata_isr);
    irq_set_priority(pio_irq_id, 0);
    cvideo_pio->inte1 = 1 << 4 + DATA_SM_ID;
}

static inline void cvdata_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint data_pin) {
    pio_sm_config c = cvdata_program_get_default_config(offset);

    // Map the state machine's OUT and SIDE pin group to one pin, namely the `pin`
    // parameter to this function.
    sm_config_set_set_pins(&c, data_pin, 1);
    sm_config_set_out_pins(&c, data_pin, 1);

    sm_config_set_clkdiv(&c, clockdiv);

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(pio, data_pin);

    // Set the pin direction to output at the PIO
    pio_sm_set_consecutive_pindirs(pio, sm, data_pin, 1, true);

    // Enable autopull and set threshold to 32 bits
    // Note that register positions are the same for all state machines
    c.shiftctrl |= (1u<<PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) |
                  (0u<<PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB);

    
    // Load our configuration, and jump to the start of the program
    pio_sm_init(pio, sm, offset, &c);

    // Tell the state machine the number of pixels per line (minus 1)
    pio_sm_put(pio, sm, CVIDEO_PIX_PER_LINE - 1);
    pio_sm_exec(pio, sm, 0x80a0);  // pull
    pio_sm_exec(pio, sm, 0xa027);  // mov    x, osr 
    pio_sm_exec(pio, sm, 0x6060);  // out null 32 ; Discard OSR contents after copying to x

    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

static inline void cvsync_program_init(PIO pio, uint sm, uint offset, float clockdiv, uint sync_pin) {
    pio_sm_config c = cvsync_program_get_default_config(offset);

    // Map the state machine's OUT and SIDE pin group to one pin, namely the `pin`
    // parameter to this function.
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
    pio_sm_put(pio, sm, (CVIDEO_LINES / 2) - 1);
    pio_sm_exec(pio, sm, 0x80a0);  // pull side 0

    // Set the state machine running
    pio_sm_set_enabled(pio, sm, true);
}

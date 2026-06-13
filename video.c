#include "video.h"

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "pico_hdmi/video_output_rt.h"
#include "pico_hdmi/hstx_data_island_queue.h"

// 16:9 480p 61Hz
const video_mode_t video_mode_480_wide_cvt = {
    .h_front_porch = 24,
    .h_sync_width = 80,
    .h_back_porch = 104,
    .h_active_pixels = 848,
    .v_front_porch = 3,
    .v_sync_width = 10,
    .v_back_porch = 7,
    .v_active_lines = 480,
    .h_total_pixels = 1056,
    .v_total_lines = 500,
    .hstx_clk_div = 1,
    .hstx_csr_clkdiv = 5,
    .sync_positive = false,
    .data_island_in_hsync = true,
};

void init_video(void)
{
    i2c_init(i2c1, 100000);
    gpio_set_function(20, GPIO_FUNC_I2C); // SDA
    gpio_set_function(21, GPIO_FUNC_I2C); // SCL
    gpio_pull_up(20);
    gpio_pull_up(21);
    hstx_di_queue_init();
    const video_mode_t *vm = &video_mode_480_wide_cvt;
    video_output_set_mode(vm);
    video_output_init(vm->h_active_pixels, vm->v_active_lines);
}
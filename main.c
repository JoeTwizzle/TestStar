#include "pico_hdmi/hstx_data_island_queue.h"
#include "pico_hdmi/hstx_packet.h"
#include "audio.h"
#include "video.h"

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/clocks.h"
#include "hardware/vreg.h"
#include "hardware/i2c.h"

#include <math.h>
#include <string.h>

#include "pico_hdmi/video_output_rt.h"
// ============================================================================
// Configuration
// ============================================================================

#define FRAME_WIDTH 854
#define FRAME_HEIGHT 480
#define BOX_SIZE 16

#define BG_COLOR 0x0010  // Dark blue (RGB565)
#define BOX_COLOR 0xFFE0 // Yellow (RGB565)

// ============================================================================
// Animation State
// ============================================================================

static volatile int box_x = 50, box_y = 50;
static int box_dx = 4, box_dy = 4;

// ============================================================================
// Scanline Callback (runs on Core 1)
// ============================================================================

static void __scratch_x("") scanline_callback(uint32_t v_scanline, uint32_t active_line, uint32_t *dst)
{
    (void)v_scanline;

    int fb_line = active_line;

    // Read current box position
    int bx = box_x;
    int by = box_y;
    uint8_t gray = ((fb_line + video_frame_count) * 255) / (FRAME_HEIGHT - 1);

    uint16_t bgCol = ((gray >> 3) << 11) | ((gray >> 2) << 5) | (gray >> 3);
    uint32_t bg = bgCol | ((uint32_t)bgCol << 16);
    uint32_t box = BOX_COLOR | (BOX_COLOR << 16);

    // Check if this line intersects the box vertically
    if (fb_line >= by && fb_line < by + BOX_SIZE)
    {
        // Three regions: before box, box, after box
        int i = 0;

        // Region 1: before box
        // Note: iterating by 2 pixels at a time (1 uint32_t)
        for (; i < bx / 2; i++)
        {
            dst[i] = bg;
        }

        // Region 2: box
        for (; i < (bx + BOX_SIZE) / 2 && i < FRAME_WIDTH / 2; i++)
        {
            dst[i] = box;
        }

        // Region 3: after box
        for (; i < FRAME_WIDTH / 2; i++)
        {
            dst[i] = bg;
        }
    }
    else
    {
        // Fast path: entire line is background
        for (int i = 0; i < FRAME_WIDTH / 2; i++)
        {
            dst[i] = bg;
        }
    }
}

// ============================================================================
// Main (Core 0)
// ============================================================================

static void update_box(void)
{
    int x = box_x + box_dx;
    int y = box_y + box_dy;

    if (x <= 0 || x + BOX_SIZE / 2 >= FRAME_WIDTH)
    {
        box_dx = -box_dx;
        x = box_x + box_dx;
    }
    if (y <= 0 || y + BOX_SIZE / 2 >= FRAME_HEIGHT)
    {
        box_dy = -box_dy;
        y = box_y + box_dy;
    }

    box_x = x;
    box_y = y;
}

int main(void)
{
#ifdef VIDEO_MODE_1280x720
    // 720p60: 372 MHz at 1.3V. Closest achievable to 371.25 MHz with 12 MHz XOSC
    // (0.2% high -> 74.4 MHz pixel clock, within HDMI tolerance for 720p60).
    vreg_set_voltage(VREG_VOLTAGE_1_30);
    sleep_ms(10);
    set_sys_clock_khz(372000, true);
#else
    // 480p60: set system clock to 126 MHz for HSTX timing (25.2 MHz pixel clock).
    set_sys_clock_khz(160000, true);
#endif
    stdio_init_all();
    sleep_ms(1000);

    // Initialize audio
    init_audio();

    // Initialize HDMI output
    init_video();

    // Register scanline callback
    video_output_set_scanline_callback(scanline_callback);

    // Pre-fill audio buffer
    generate_audio();

    // Launch Core 1 for HSTX output
    multicore_launch_core1(video_output_core1_run);
    sleep_ms(100);

    // Main loop - animation + audio
    uint32_t last_frame = 0;
    bool led_state = false;

    while (true)
    {
        // Keep audio buffer fed
        generate_audio();

        // Same frame
        while (video_frame_count == last_frame)
        {
            generate_audio();
            tight_loop_contents();
        }
        last_frame = video_frame_count;

        // Update animation
        update_box();

        // Advance melody (one note step per frame)
        advance_melody();
    }

    return 0;
}

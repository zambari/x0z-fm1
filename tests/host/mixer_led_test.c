/* SPDX-License-Identifier: GPL-3.0-only */
/* fm1 fork: mixer LEDs follow the rendered bars, including silence, mutes and held pads. */
#define main x0x_host_main
#include "../../host/x0x_host.c"
#undef main
#include <assert.h>

int main(void)
{
    int h, full, p;
    boot();
    for (full = 64; full <= 96; full += 32) {
        int prev = 0;
        for (h = 0; h <= full; h++) {
            int duty = mix_led_duty(h, full);
            assert(duty >= prev && duty <= 4);
            assert(h > 1 || duty == 0);
            prev = duty;
        }
        assert(mix_led_duty(full / 4, full) == 1);
        assert(mix_led_duty(full / 2, full) == 2);
        assert(mix_led_duty(full * 3 / 4, full) == 3);
        assert(mix_led_duty(full, full) == 4);
    }
    ui.view = V_MIX;
    build_pages();
    proj.set.keyled = 1;
    seq.mute = seq.vmute = 0;
    for (p = 0; p < NPARTS; p++)
        eng_peak[p] = p == 4 ? 32767 : p * 8192;
    draw_main();
    for (p = 0; p < NPARTS; p++)
        assert(mix_led_level[p] == p);
    /* An audio IRQ can raise the live peak after drawing: keep the displayed snapshot. */
    eng_peak[0] = 32767;
    ui.keys = 1u << BLACK_KEY[0];
    leds();
    assert(!((lit_keys | dim_keys) & ui.keys));
    assert(lit_keys & (1u << BLACK_KEY[4]));
    seq.mute = MUTE_MASK[4];
    draw_main();
    leds();
    assert(!((lit_keys | dim_keys) & (1u << BLACK_KEY[4])));
    /* Drum bars use hit velocity times the level fader, with voice mute respected. */
    *page_of(V_MIX) = 1;
    build_pages();
    assert(drum_mix_page() == 0);
    pref_t level = find_ref(T_909, 0, "Level");
    assert(level.kind);
    proj.sound.v[T_909][0][level.c] = 127;
    eng_hit[0][0] = 255;
    draw_main();
    assert(mix_led_level[0] == 4);
    seq.vmute = 1u;
    draw_main();
    assert(mix_led_level[0] == 0);
    seq.vmute = 0;
    eng_hit[0][0] = 0;
    draw_main();
    assert(mix_led_level[0] == 0);
    /* Leaving MIX restores the usual key indicators and held-key feedback. */
    ui.view = V_FX;
    build_pages();
    leds();
    assert(lit_keys == ui.keys && dim_keys == 0);
    puts("mixer LED levels, displayed snapshot, mutes and page transitions: OK");
    return 0;
}

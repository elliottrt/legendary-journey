#include "kernel/drivers/kbd.h"
#include "kernel/cpu/irq.h"
#include "kernel/graphics/printf.h"
#include "kernel/memory/kalloc.h"
#include "kernel/util/ringbuf.h"
#include "common/mmu.h"
#include "common/std.h"

uint8_t keyboard_us[2][128] = {
    {
        KEY_NULL, KEY_ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
        '-', '=', KEY_BACKSPACE, KEY_TAB, 'q', 'w', 'e', 'r', 't', 'y', 'u',
        'i', 'o', 'p', '[', ']', KEY_ENTER, 0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
        'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm',
        ',', '.', '/', 0, 0, 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5,
        KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP,
        KEY_PAGE_UP, '-', KEY_LEFT, '5', KEY_RIGHT, '+', KEY_END, KEY_DOWN,
        KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12
    }, {
        KEY_NULL, KEY_ESC, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
        '_', '+', KEY_BACKSPACE, KEY_TAB, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U',
        'I', 'O', 'P', '{', '}', KEY_ENTER, 0, 'A', 'S', 'D', 'F', 'G', 'H',
        'J', 'K', 'L', ':', '\"', '~', 0, '|', 'Z', 'X', 'C', 'V', 'B', 'N',
        'M', '<', '>', '?', 0, 0, 0, ' ', 0, KEY_F1, KEY_F2, KEY_F3, KEY_F4,
        KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 0, 0, KEY_HOME, KEY_UP,
        KEY_PAGE_UP, '-', KEY_LEFT, '5', KEY_RIGHT, '+', KEY_END, KEY_DOWN,
        KEY_PAGE_DOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, KEY_F11, KEY_F12
    }
};

struct keyboard keyboard = {0};
struct ringbuf key_input_buf = {0};

char getkeychar(uint16_t scancode)
{
    uint32_t shift = KEY_MOD(keyboard.mods, KEY_MOD_SHIFT) ? 1 : 0;
    return KEY_SCANCODE(scancode) < 128 ? keyboard_us[shift][KEY_SCANCODE(scancode)] : 0;
}

#define HIBIT(_x) (31 - __builtin_clz((_x)))
static void kbdhandler(struct registers *regs) {
    UNUSED(regs);

    uint16_t scancode = (uint16_t) inb(0x60);

    if (KEY_SCANCODE(scancode) == KEY_LALT ||
        KEY_SCANCODE(scancode) == KEY_RALT) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_ALT), KEY_PRESSED(scancode));
    } else if (
        KEY_SCANCODE(scancode) == KEY_LCTRL ||
        KEY_SCANCODE(scancode) == KEY_RCTRL) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_CTRL), KEY_PRESSED(scancode));
    } else if (
        KEY_SCANCODE(scancode) == KEY_LSHIFT ||
        KEY_SCANCODE(scancode) == KEY_RSHIFT) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_SHIFT), KEY_PRESSED(scancode));
    } else if (KEY_SCANCODE(scancode) == KEY_CAPS_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_CAPS_LOCK), KEY_PRESSED(scancode));
    } else if (KEY_SCANCODE(scancode) == KEY_NUM_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_NUM_LOCK), KEY_PRESSED(scancode));
    } else if (KEY_SCANCODE(scancode) == KEY_SCROLL_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_SCROLL_LOCK), KEY_PRESSED(scancode));
    }

    char keychar = getkeychar(scancode);

    keyboard.keys[KEY_SCANCODE(scancode)] = KEY_PRESSED(scancode);
    keyboard.chars[(uint8_t) keychar] = KEY_PRESSED(scancode);

    if (KEY_PRESSED(scancode) && keychar != 0)
        ringbuf_put(&key_input_buf, &keychar, sizeof(keychar));
}

void kbdinit(void) 
{
    if ((key_input_buf.buffer = kalloc()) == NULL) {
        printf("kbd: error: unable to allocate memory: %s\n", strerror(errno));
    }

    key_input_buf.cap = PGSIZE;
    key_input_buf.start = 0;
    key_input_buf.end = 0;

	irqinstall(1, kbdhandler);
}

char kbd_getc(void) {
    char ch = KEY_NULL;

    // might fail, in which case it won't modify ch
    ringbuf_take(&key_input_buf, &ch, sizeof(ch));

    return ch;
}

char kbd_getc_blocking(void) {
    char ch = KEY_NULL;

    while (ch == KEY_NULL) {
        // halt until an interrupt, then try to get a key
        // which would be populated by a keyboard interrupt

        halt();

        ch = kbd_getc();
    }

    return ch;
}

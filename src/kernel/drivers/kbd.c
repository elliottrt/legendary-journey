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

unsigned char getkeychar(uint16_t scancode)
{
    uint32_t shift = KEY_MOD(keyboard.mods, KEY_MOD_SHIFT) ? 1 : 0;
    return KEY_SCANCODE(scancode) < 128 ? keyboard_us[shift][KEY_SCANCODE(scancode)] : 0;
}

#define HIBIT(_x) (31 - __builtin_clz((_x)))
static void kbdhandler(struct registers *regs) {
    UNUSED(regs);

    uint16_t scancode = (uint16_t) inb(0x60);
    uint8_t key_code = KEY_SCANCODE(scancode);
    bool key_pressed = KEY_PRESSED(scancode);

    if (key_code == KEY_LALT || key_code == KEY_RALT) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_ALT), key_pressed);

    } else if (key_code == KEY_LCTRL || key_code == KEY_RCTRL) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_CTRL), key_pressed);

    } else if (key_code == KEY_LSHIFT || key_code == KEY_RSHIFT) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_SHIFT), key_pressed);

    } else if (key_code == KEY_CAPS_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_CAPS_LOCK), key_pressed);

    } else if (key_code == KEY_NUM_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_NUM_LOCK), key_pressed);

    } else if (key_code == KEY_SCROLL_LOCK) {
        keyboard.mods = BIT_SET(keyboard.mods, HIBIT(KEY_MOD_SCROLL_LOCK), key_pressed);
    }

    unsigned char key_char = getkeychar(scancode);

    keyboard.keys[key_code] = key_pressed;
    keyboard.chars[(int) key_char] = key_pressed;

    // note: if key_code == key_char then it is special char
    // we use the high bit to indicate if it is ascii
    if (key_pressed && key_char != 0) {
        if (key_code == key_char) key_char |= 0x80;
        ringbuf_put(&key_input_buf, &key_char, sizeof(key_char));
    }
}

void kbdinit(void) 
{
    if ((key_input_buf.buffer = kalloc()) == NULL) {
        printf("kbd: error: unable to allocate memory: %m\n");
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

    while ((ch = kbd_getc()) == KEY_NULL) {
        // halt until an interrupt, then try to get a key
        // which would be populated by a keyboard interrupt
        halt();
    }

    return ch;
}

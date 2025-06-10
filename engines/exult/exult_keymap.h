#ifndef EXULT_KEYMAP_H
#define EXULT_KEYMAP_H

#include "common/events.h" // For Common::Keycode, Common::KeyModifier

namespace ScummVM {
namespace Exult {

// Minimal SDL1.2-like key definitions.
// These should correspond to the values Exult's KeyBinder expects.
// Based on SDL_keysym.h from SDL 1.2.x and Exult's own key string table.
namespace ExultSDL12 {
    enum Key {
        SDLK_UNKNOWN = 0,
        SDLK_BACKSPACE = 8,
        SDLK_TAB = 9,
        SDLK_RETURN = 13,
        SDLK_ESCAPE = 27,
        SDLK_SPACE = 32,

        SDLK_0 = 48, SDLK_1 = 49, SDLK_2 = 50, SDLK_3 = 51, SDLK_4 = 52,
        SDLK_5 = 53, SDLK_6 = 54, SDLK_7 = 55, SDLK_8 = 56, SDLK_9 = 57,

        SDLK_a = 97, SDLK_b = 98, SDLK_c = 99, SDLK_d = 100, SDLK_e = 101,
        SDLK_f = 102, SDLK_g = 103, SDLK_h = 104, SDLK_i = 105, SDLK_j = 106,
        SDLK_k = 107, SDLK_l = 108, SDLK_m = 109, SDLK_n = 110, SDLK_o = 111,
        SDLK_p = 112, SDLK_q = 113, SDLK_r = 114, SDLK_s = 115, SDLK_t = 116,
        SDLK_u = 117, SDLK_v = 118, SDLK_w = 119, SDLK_x = 120, SDLK_y = 121,
        SDLK_z = 122,
        SDLK_DELETE = 127,

        // Keypad
        SDLK_KP_0 = 256, SDLK_KP_1 = 257, SDLK_KP_2 = 258, SDLK_KP_3 = 259, SDLK_KP_4 = 260,
        SDLK_KP_5 = 261, SDLK_KP_6 = 262, SDLK_KP_7 = 263, SDLK_KP_8 = 264, SDLK_KP_9 = 265,
        SDLK_KP_PERIOD = 266, SDLK_KP_DIVIDE = 267, SDLK_KP_MULTIPLY = 268,
        SDLK_KP_MINUS = 269, SDLK_KP_PLUS = 270, SDLK_KP_ENTER = 271, SDLK_KP_EQUALS = 272,

        // Arrows + Home/End Function keys
        SDLK_UP = 273, SDLK_DOWN = 274, SDLK_RIGHT = 275, SDLK_LEFT = 276,
        SDLK_INSERT = 277, SDLK_HOME = 278, SDLK_END = 279, SDLK_PAGEUP = 280, SDLK_PAGEDOWN = 281,

        SDLK_F1 = 282, SDLK_F2 = 283, SDLK_F3 = 284, SDLK_F4 = 285, SDLK_F5 = 286,
        SDLK_F6 = 287, SDLK_F7 = 288, SDLK_F8 = 289, SDLK_F9 = 290, SDLK_F10 = 291,
        SDLK_F11 = 292, SDLK_F12 = 293, SDLK_F13 = 294, SDLK_F14 = 295, SDLK_F15 = 296,

        // TODO: Add other keys like punctuation, more control keys if Exult uses them.
        // For example: comma, period, slash, semicolon, quote, brackets, backquote etc.
        // Based on SDLKeyStringTable from keys.cc:
        SDLK_PAUSE = 19, // From SDL_keysym.h, not in string table but common

        // These are not SDL1.2 standard values but rather placeholders if mapping is complex
        SDLK_LAST // Sentinel
    };

    // SDL1.2 Modifiers
    enum Mod {
        KMOD_NONE = 0x0000,
        KMOD_LSHIFT = 0x0001,
        KMOD_RSHIFT = 0x0002,
        KMOD_LCTRL = 0x0040,
        KMOD_RCTRL = 0x0080,
        KMOD_LALT = 0x0100,
        KMOD_RALT = 0x0200,
        KMOD_LMETA = 0x0400, // Corresponds to LSUPER/Command on SDL2
        KMOD_RMETA = 0x0800, // Corresponds to RSUPER/Command on SDL2
        KMOD_NUM = 0x1000,
        KMOD_CAPS = 0x2000,
        KMOD_MODE = 0x4000, // AltGr
        KMOD_RESERVED = 0x8000,

        // Combinations commonly used by SDL1.2 applications
        KMOD_CTRL = (KMOD_LCTRL | KMOD_RCTRL),
        KMOD_SHIFT = (KMOD_LSHIFT | KMOD_RSHIFT),
        KMOD_ALT = (KMOD_LALT | KMOD_RALT),
        KMOD_META = (KMOD_LMETA | KMOD_RMETA)
    };

    // SDL1.2 Scancode was often just a direct mapping for many keys or an internal enum.
    // Exult's KeyBinder seems to primarily use keysym.sym and keysym.mod.
    // If scancode is truly needed and differs from sym for some Exult logic,
    // this would need a more elaborate mapping. For now, we can try to pass a casted sym.
    typedef uint8_t Scancode; // SDL1.2 scancode was often uint8.
} // namespace ExultSDL12

ExultSDL12::Key scummvmKeyToExultSDL12Key(Common::Keycode scummvmKey);
ExultSDL12::Mod scummvmModToExultSDL12Mod(Common::KeyModifier scummvmMod);
// Scancode mapping might be trivial (cast) or complex.
// Exult's KeyBinder uses event.key.keysym.sym primarily.
// Let's assume scancode can be derived from sym if needed, or is not strictly used by Exult's KeyBinder logic
// beyond what sym provides for action mapping.
// uint8_t scummvmScancodeToExultSDL12Scancode(uint16_t scummvmScancode);

} // namespace Exult
} // namespace ScummVM

#endif // EXULT_KEYMAP_H

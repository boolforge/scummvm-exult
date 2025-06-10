#include "exult_keymap.h"

// For debug
#include "common/debug.h"

namespace ScummVM {
namespace Exult {

ExultSDL12::Key scummvmKeyToExultSDL12Key(Common::Keycode scummvmKey) {
    // This is a large switch statement.
    // Mapping based on SDL2 Keycodes (from Common::Keycode which mirrors SDL_Keycode)
    // to SDL1.2 Keycodes (our ExultSDL12::Key enum).
    // Many common keys have the same values, but many differ.
    // This list needs to be comprehensive for keys Exult might use.
    switch (scummvmKey) {
        case Common::Keycode::AK_UNKNOWN: return ExultSDL12::SDLK_UNKNOWN;
        case Common::Keycode::AK_BACKSPACE: return ExultSDL12::SDLK_BACKSPACE;
        case Common::Keycode::AK_TAB: return ExultSDL12::SDLK_TAB;
        case Common::Keycode::AK_RETURN: return ExultSDL12::SDLK_RETURN;
        case Common::Keycode::AK_ESCAPE: return ExultSDL12::SDLK_ESCAPE;
        case Common::Keycode::AK_SPACE: return ExultSDL12::SDLK_SPACE;

        case Common::Keycode::AK_0: return ExultSDL12::SDLK_0;
        case Common::Keycode::AK_1: return ExultSDL12::SDLK_1;
        case Common::Keycode::AK_2: return ExultSDL12::SDLK_2;
        case Common::Keycode::AK_3: return ExultSDL12::SDLK_3;
        case Common::Keycode::AK_4: return ExultSDL12::SDLK_4;
        case Common::Keycode::AK_5: return ExultSDL12::SDLK_5;
        case Common::Keycode::AK_6: return ExultSDL12::SDLK_6;
        case Common::Keycode::AK_7: return ExultSDL12::SDLK_7;
        case Common::Keycode::AK_8: return ExultSDL12::SDLK_8;
        case Common::Keycode::AK_9: return ExultSDL12::SDLK_9;

        case Common::Keycode::AK_A: return ExultSDL12::SDLK_a;
        case Common::Keycode::AK_B: return ExultSDL12::SDLK_b;
        case Common::Keycode::AK_C: return ExultSDL12::SDLK_c;
        case Common::Keycode::AK_D: return ExultSDL12::SDLK_d;
        case Common::Keycode::AK_E: return ExultSDL12::SDLK_e;
        case Common::Keycode::AK_F: return ExultSDL12::SDLK_f;
        case Common::Keycode::AK_G: return ExultSDL12::SDLK_g;
        case Common::Keycode::AK_H: return ExultSDL12::SDLK_h;
        case Common::Keycode::AK_I: return ExultSDL12::SDLK_i;
        case Common::Keycode::AK_J: return ExultSDL12::SDLK_j;
        case Common::Keycode::AK_K: return ExultSDL12::SDLK_k;
        case Common::Keycode::AK_L: return ExultSDL12::SDLK_l;
        case Common::Keycode::AK_M: return ExultSDL12::SDLK_m;
        case Common::Keycode::AK_N: return ExultSDL12::SDLK_n;
        case Common::Keycode::AK_O: return ExultSDL12::SDLK_o;
        case Common::Keycode::AK_P: return ExultSDL12::SDLK_p;
        case Common::Keycode::AK_Q: return ExultSDL12::SDLK_q;
        case Common::Keycode::AK_R: return ExultSDL12::SDLK_r;
        case Common::Keycode::AK_S: return ExultSDL12::SDLK_s;
        case Common::Keycode::AK_T: return ExultSDL12::SDLK_t;
        case Common::Keycode::AK_U: return ExultSDL12::SDLK_u;
        case Common::Keycode::AK_V: return ExultSDL12::SDLK_v;
        case Common::Keycode::AK_W: return ExultSDL12::SDLK_w;
        case Common::Keycode::AK_X: return ExultSDL12::SDLK_x;
        case Common::Keycode::AK_Y: return ExultSDL12::SDLK_y;
        case Common::Keycode::AK_Z: return ExultSDL12::SDLK_z;

        case Common::Keycode::AK_DELETE: return ExultSDL12::SDLK_DELETE;
        case Common::Keycode::AK_PAUSE: return ExultSDL12::SDLK_PAUSE;

        // Keypad
        case Common::Keycode::AK_KP_0: return ExultSDL12::SDLK_KP_0;
        case Common::Keycode::AK_KP_1: return ExultSDL12::SDLK_KP_1;
        case Common::Keycode::AK_KP_2: return ExultSDL12::SDLK_KP_2;
        case Common::Keycode::AK_KP_3: return ExultSDL12::SDLK_KP_3;
        case Common::Keycode::AK_KP_4: return ExultSDL12::SDLK_KP_4;
        case Common::Keycode::AK_KP_5: return ExultSDL12::SDLK_KP_5;
        case Common::Keycode::AK_KP_6: return ExultSDL12::SDLK_KP_6;
        case Common::Keycode::AK_KP_7: return ExultSDL12::SDLK_KP_7;
        case Common::Keycode::AK_KP_8: return ExultSDL12::SDLK_KP_8;
        case Common::Keycode::AK_KP_9: return ExultSDL12::SDLK_KP_9;
        case Common::Keycode::AK_KP_PERIOD: return ExultSDL12::SDLK_KP_PERIOD;
        case Common::Keycode::AK_KP_DIVIDE: return ExultSDL12::SDLK_KP_DIVIDE;
        case Common::Keycode::AK_KP_MULTIPLY: return ExultSDL12::SDLK_KP_MULTIPLY;
        case Common::Keycode::AK_KP_MINUS: return ExultSDL12::SDLK_KP_MINUS;
        case Common::Keycode::AK_KP_PLUS: return ExultSDL12::SDLK_KP_PLUS;
        case Common::Keycode::AK_KP_ENTER: return ExultSDL12::SDLK_KP_ENTER;
        // Common::Keycode::AK_KP_EQUALS often maps to SDLK_KP_EQUALS (272 in SDL1.2)
        // ExultSDL12::SDLK_KP_EQUALS is defined as 272.
        case Common::Keycode::AK_KP_EQUALS: return ExultSDL12::SDLK_KP_EQUALS;


        // Arrows + Home/End Function keys
        case Common::Keycode::AK_UP: return ExultSDL12::SDLK_UP;
        case Common::Keycode::AK_DOWN: return ExultSDL12::SDLK_DOWN;
        case Common::Keycode::AK_RIGHT: return ExultSDL12::SDLK_RIGHT;
        case Common::Keycode::AK_LEFT: return ExultSDL12::SDLK_LEFT;
        case Common::Keycode::AK_INSERT: return ExultSDL12::SDLK_INSERT;
        case Common::Keycode::AK_HOME: return ExultSDL12::SDLK_HOME;
        case Common::Keycode::AK_END: return ExultSDL12::SDLK_END;
        case Common::Keycode::AK_PAGEUP: return ExultSDL12::SDLK_PAGEUP;
        case Common::Keycode::AK_PAGEDOWN: return ExultSDL12::SDLK_PAGEDOWN;

        case Common::Keycode::AK_F1: return ExultSDL12::SDLK_F1;
        case Common::Keycode::AK_F2: return ExultSDL12::SDLK_F2;
        case Common::Keycode::AK_F3: return ExultSDL12::SDLK_F3;
        case Common::Keycode::AK_F4: return ExultSDL12::SDLK_F4;
        case Common::Keycode::AK_F5: return ExultSDL12::SDLK_F5;
        case Common::Keycode::AK_F6: return ExultSDL12::SDLK_F6;
        case Common::Keycode::AK_F7: return ExultSDL12::SDLK_F7;
        case Common::Keycode::AK_F8: return ExultSDL12::SDLK_F8;
        case Common::Keycode::AK_F9: return ExultSDL12::SDLK_F9;
        case Common::Keycode::AK_F10: return ExultSDL12::SDLK_F10;
        case Common::Keycode::AK_F11: return ExultSDL12::SDLK_F11;
        case Common::Keycode::AK_F12: return ExultSDL12::SDLK_F12;
        // SDL1.2 only goes up to F15 typically
        case Common::Keycode::AK_F13: return ExultSDL12::SDLK_F13;
        case Common::Keycode::AK_F14: return ExultSDL12::SDLK_F14;
        case Common::Keycode::AK_F15: return ExultSDL12::SDLK_F15;

        // Punctuation (examples, more would be needed)
        // These mappings are particularly tricky as SDL1.2 and SDL2 differ significantly here.
        // The values in ExultSDL12::Key for these are often direct ASCII.
        case Common::Keycode::AK_COMMA: return (ExultSDL12::Key)','; // SDLK_COMMA in SDL1.2 is 44 (ASCII)
        case Common::Keycode::AK_PERIOD: return (ExultSDL12::Key)'.'; // SDLK_PERIOD is 46
        case Common::Keycode::AK_SLASH: return (ExultSDL12::Key)'/';   // SDLK_SLASH is 47
        case Common::Keycode::AK_SEMICOLON: return (ExultSDL12::Key)';';// SDLK_SEMICOLON is 59
        case Common::Keycode::AK_QUOTE: return (ExultSDL12::Key)'\''; // SDLK_QUOTE is 39
        case Common::Keycode::AK_LEFTBRACKET: return (ExultSDL12::Key)'['; // SDLK_LEFTBRACKET is 91
        case Common::Keycode::AK_RIGHTBRACKET: return (ExultSDL12::Key)']'; // SDLK_RIGHTBRACKET is 93
        case Common::Keycode::AK_BACKQUOTE: return (ExultSDL12::Key)'`'; // SDLK_BACKQUOTE is 96
        case Common::Keycode::AK_MINUS: return (ExultSDL12::Key)'-'; // SDLK_MINUS is 45
        case Common::Keycode::AK_EQUALS: return (ExultSDL12::Key)'='; // SDLK_EQUALS is 61
        case Common::Keycode::AK_BACKSLASH: return (ExultSDL12::Key)'\\'; // SDLK_BACKSLASH is 92


        // TODO: Add many more mappings (CAPSLOCK, NUMLOCK, SCROLLLOCK, PrintScreen, etc.)
        // if Exult uses them. Many control keys like LCTRL, LSHIFT are handled by modifiers.

        default:
            // This logs to ScummVM's console, which is good for debugging unmapped keys.
            ScummVM::debug(2, "scummvmKeyToExultSDL12Key: Unmapped ScummVM Keycode: %d (0x%X)", (int)scummvmKey, (int)scummvmKey);
            return ExultSDL12::SDLK_UNKNOWN;
    }
}

ExultSDL12::Mod scummvmModToExultSDL12Mod(Common::KeyModifier scummvmMod) {
    uint16_t exultMod = ExultSDL12::KMOD_NONE;
    if (scummvmMod & Common::KeyModifier::LSHIFT) exultMod |= ExultSDL12::KMOD_LSHIFT;
    if (scummvmMod & Common::KeyModifier::RSHIFT) exultMod |= ExultSDL12::KMOD_RSHIFT;
    if (scummvmMod & Common::KeyModifier::LCTRL)  exultMod |= ExultSDL12::KMOD_LCTRL;
    if (scummvmMod & Common::KeyModifier::RCTRL)  exultMod |= ExultSDL12::KMOD_RCTRL;
    if (scummvmMod & Common::KeyModifier::LALT)   exultMod |= ExultSDL12::KMOD_LALT;
    if (scummvmMod & Common::KeyModifier::RALT)   exultMod |= ExultSDL12::KMOD_RALT;
    if (scummvmMod & Common::KeyModifier::LMETA)  exultMod |= ExultSDL12::KMOD_LMETA; // SDL2 LSUPER/Windows/Command
    if (scummvmMod & Common::KeyModifier::RMETA)  exultMod |= ExultSDL12::KMOD_RMETA; // SDL2 RSUPER/Windows/Command
    if (scummvmMod & Common::KeyModifier::NUM)    exultMod |= ExultSDL12::KMOD_NUM;
    if (scummvmMod & Common::KeyModifier::CAPS)   exultMod |= ExultSDL12::KMOD_CAPS;
    // Common::KeyModifier::MODE (AltGr) maps to ExultSDL12::KMOD_MODE
    if (scummvmMod & Common::KeyModifier::MODE)   exultMod |= ExultSDL12::KMOD_MODE;

    // SDL1.2 applications often checked combined KMOD_SHIFT, KMOD_CTRL, KMOD_ALT.
    // The individual L/R flags are usually sufficient, but Exult's KeyBinder logic might use the combined ones.
    // The ExultSDL12::Mod enum already defines KMOD_CTRL, KMOD_SHIFT, KMOD_ALT as combinations.
    // Exult's KeyBinder specifically checks against KMOD_SHIFT, KMOD_CTRL, KMOD_ALT (combined).
    // So, if any L/R is set, the combined should also be implicitly there if KeyBinder uses bitwise checks correctly.
    // For safety or direct match with Exult's KeyBinder logic which uses `key.mod = KMOD_NONE; if (ev.key.keysym.mod & KMOD_SHIFT) key.mod |= KMOD_SHIFT; ...`
    // we can ensure the combined flags are set if individuals are.
    if (exultMod & (ExultSDL12::KMOD_LSHIFT | ExultSDL12::KMOD_RSHIFT)) exultMod |= ExultSDL12::KMOD_SHIFT;
    if (exultMod & (ExultSDL12::KMOD_LCTRL | ExultSDL12::KMOD_RCTRL)) exultMod |= ExultSDL12::KMOD_CTRL;
    if (exultMod & (ExultSDL12::KMOD_LALT | ExultSDL12::KMOD_RALT)) exultMod |= ExultSDL12::KMOD_ALT;
    if (exultMod & (ExultSDL12::KMOD_LMETA | ExultSDL12::KMOD_RMETA)) exultMod |= ExultSDL12::KMOD_META;


    return (ExultSDL12::Mod)exultMod;
}

// Scancode mapping: Exult's KeyBinder::TranslateEvent uses keysym.sym and keysym.mod.
// The `scancode` field in SDL_keysym was often 0 or a direct mapping from the hardware,
// but less consistently used for game logic binding than `sym`.
// If Exult's KeyBinder *does* use `scancode` distinctively, a proper mapping from
// SDL2 scancodes (Common::ScanCode, which are SDL_Scancode) to SDL1.2 scancodes would be needed.
// For now, we'll assume it's not the primary factor for Exult's key binding logic.
// A placeholder might be:
/*
uint8_t scummvmScancodeToExultSDL12Scancode(uint16_t scummvmScancode) {
    // This is highly dependent on how Exult uses scancodes.
    // A direct cast is unlikely to be correct.
    // Many SDL1.2 games didn't rely heavily on scancodes for abstract key bindings.
    // Returning 0 or a cast of a mapped sym might be safer if Exult doesn't really use it.
    return static_cast<uint8_t>(scummvmScancode & 0xFF); // Simplistic, likely incorrect.
}
*/

} // namespace Exult
} // namespace ScummVM

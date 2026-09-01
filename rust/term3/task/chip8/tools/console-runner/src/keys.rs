const N_KEYS: usize = chip8::Key::MAX.as_usize() + 1;

pub const INDEX: [u8; N_KEYS] = [
    0x1, 0x2, 0x3, 0xC, //
    0x4, 0x5, 0x6, 0xD, //
    0x7, 0x8, 0x9, 0xE, //
    0xA, 0x0, 0xB, 0xF, //
];

pub const SYMBOLS: [&str; N_KEYS] = [
    "1", "2", "3", "C", //
    "4", "5", "6", "D", //
    "7", "8", "9", "E", //
    "A", "0", "B", "F", //
];

const KEYBOARD_CHARS: [char; N_KEYS] = [
    '1', '2', '3', '4', //
    'q', 'w', 'e', 'r', //
    'a', 's', 'd', 'f', //
    'z', 'x', 'c', 'v', //
];

pub fn is_keyboard_char(c: char) -> Option<usize> {
    KEYBOARD_CHARS.iter().position(|&key| key == c)
}

use device_query::Keycode::*;
const KEYCODES: [device_query::Keycode; N_KEYS] = [
    Key1, Key2, Key3, Key4, //
    Q, W, E, R, //
    A, S, D, F, //
    Z, X, C, V, //
];

pub fn is_keyboard_keycode(c: device_query::Keycode) -> Option<usize> {
    KEYCODES.iter().position(|&key| key == c)
}

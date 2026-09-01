mod widgets;

use std::time::Duration;

use device_query::{DeviceQuery, DeviceState};
use ratatui::crossterm::event::{KeyCode, KeyEvent};

use crate::keys;

#[derive(Default)]
pub struct Keyboard([bool; chip8::Key::DOMAIN_SIZE]);

pub struct Runner<R: chip8::RandomNumberGenerator> {
    interpreter: chip8::ManagedInterpreter<R>,
    snapshots: Vec<chip8::ManagedInterpreter<R>>,
    error: Option<chip8::Error>,
    keyboard: Keyboard,
    sticky_keyboard: bool,
    pause: bool,
    fullscreen: bool,
    show_help: bool,
    device_state: DeviceState,
}

impl<R: chip8::RandomNumberGenerator + Clone> Runner<R> {
    pub fn new(interpreter: chip8::ManagedInterpreter<R>, debug: bool) -> Self {
        Self {
            interpreter,
            snapshots: Default::default(),
            error: None,
            keyboard: Default::default(),
            sticky_keyboard: debug,
            pause: debug,
            fullscreen: !debug,
            show_help: false,
            device_state: DeviceState::new(),
        }
    }

    pub fn handle(&mut self, e: KeyEvent) -> bool {
        match e.code {
            KeyCode::Esc => return true,
            KeyCode::Right | KeyCode::Char('n') => self.one_step(),
            KeyCode::Char('p') => self.toggle_pause(),
            KeyCode::Char('l') => self.toggle_fullscreen(),
            KeyCode::Char('k') => self.toggle_sticky(),
            KeyCode::Char('t') => self.take_snapshot(),
            KeyCode::Char('y') => self.restore_snapshot(),
            KeyCode::Char('h') => self.toggle_help(),
            KeyCode::Char(c) => {
                if let Some(i) = keys::is_keyboard_char(c) {
                    self.keypress(i);
                }
            }
            _ => {}
        };
        false
    }

    fn one_step(&mut self) {
        if self.pause {
            self.error = self.interpreter.simulate_one_instruction().err()
        }
    }

    fn take_snapshot(&mut self) {
        self.snapshots.push(self.interpreter.clone());
    }

    fn restore_snapshot(&mut self) {
        if let Some(snapshot) = self.snapshots.pop() {
            self.interpreter = snapshot;
        }
    }

    fn keypress(&mut self, i: usize) {
        if self.sticky_keyboard {
            self.keyboard.0[i] ^= true;
        }
    }

    fn toggle_help(&mut self) {
        self.show_help = !self.show_help;
    }

    fn toggle_sticky(&mut self) {
        self.sticky_keyboard = !self.sticky_keyboard;
    }

    fn toggle_fullscreen(&mut self) {
        self.fullscreen = !self.fullscreen;
    }

    fn toggle_pause(&mut self) {
        if self.pause {
            self.pause = false;
            self.sticky_keyboard = true;
        } else {
            self.pause = true;
        }
    }

    pub fn tick(&mut self, elapsed: Duration) {
        if !self.sticky_keyboard {
            self.set_current_keyboard_state();
        }
        for (i, &is_down) in self.keyboard.0.iter().enumerate() {
            self.interpreter
                .set_key_down(chip8::Key::try_from(keys::INDEX[i]).unwrap(), is_down);
        }
        if !self.pause {
            self.error = self.interpreter.simulate_duration(elapsed).err()
        }
    }

    fn set_current_keyboard_state(&mut self) {
        self.keyboard.0.iter_mut().for_each(|k| *k = false);
        for key in self.device_state.get_keys() {
            if let Some(i) = keys::is_keyboard_keycode(key) {
                self.keyboard.0[i] = true;
            }
        }
    }
}

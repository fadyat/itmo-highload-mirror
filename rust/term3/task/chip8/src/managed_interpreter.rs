use crate::{
    data::Word,
    error::Result,
    image::Image,
    interpreter::{Interpreter, SCREEN_HEIGHT, SCREEN_WIDTH},
    platform::{Key, Platform, Point, Sprite},
};

use core::time::Duration;

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone)]
pub struct FrameBuffer([[bool; SCREEN_WIDTH]; SCREEN_HEIGHT]);

impl Default for FrameBuffer {
    fn default() -> Self {
        Self([[false; SCREEN_WIDTH]; SCREEN_HEIGHT])
    }
}

impl AsRef<[[bool; SCREEN_WIDTH]; SCREEN_HEIGHT]> for FrameBuffer {
    fn as_ref(&self) -> &[[bool; SCREEN_WIDTH]; SCREEN_HEIGHT] {
        &self.0
    }
}

impl FrameBuffer {
    pub fn iter_rows(&self) -> impl Iterator<Item = &[bool; SCREEN_WIDTH]> {
        self.0.iter()
    }
}

////////////////////////////////////////////////////////////////////////////////

pub trait RandomNumberGenerator {
    fn get_random_word(&mut self) -> Word;
}

impl<R: FnMut() -> Word> RandomNumberGenerator for R {
    fn get_random_word(&mut self) -> Word {
        (self)()
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Default, Clone)]
pub struct ManagedPlatform<R: RandomNumberGenerator> {
    rand: R,
    frame_buffer: FrameBuffer,
    delay_timer: Word,
    sound_timer: Word,
    key_state: [bool; 16],
    key_press_queue: Option<Key>,
}

impl<R: RandomNumberGenerator> Platform for ManagedPlatform<R> {
    fn draw_sprite(&mut self, pos: Point, sprite: Sprite) -> bool {
        let mut collision = false;

        for pixel in sprite.iter_pixels() {
            let x = (pos.x + pixel.x) as usize;
            let y = (pos.y + pixel.y) as usize;

            if x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT {
                continue;
            }

            if self.frame_buffer.0[y][x] {
                collision = true;
            }
            self.frame_buffer.0[y][x] ^= true;
        }

        collision
    }

    fn clear_screen(&mut self) {
        self.frame_buffer = FrameBuffer::default();
    }

    fn get_delay_timer(&self) -> Word {
        self.delay_timer
    }

    fn set_delay_timer(&mut self, value: Word) {
        self.delay_timer = value;
    }

    fn set_sound_timer(&mut self, value: Word) {
        self.sound_timer = value;
    }

    fn is_key_down(&self, key: Key) -> bool {
        self.key_state[key.as_usize()]
    }

    fn consume_key_press(&mut self) -> Option<Key> {
        self.key_press_queue.take()
    }

    fn get_random_word(&mut self) -> Word {
        self.rand.get_random_word()
    }
}

impl<R: RandomNumberGenerator> ManagedPlatform<R> {
    fn new(rand: R) -> Self {
        Self {
            rand,
            frame_buffer: Default::default(),
            delay_timer: 0,
            sound_timer: 0,
            key_state: [false; 16],
            key_press_queue: None,
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone)]
pub struct ManagedInterpreter<R: RandomNumberGenerator> {
    inner: Interpreter<ManagedPlatform<R>>,
    operation_duration: Duration,
    delay_tick_duration: Duration,
    sound_tick_duration: Duration,
    delay_accumulated: Duration,
    sound_accumulated: Duration,
}

impl<R: RandomNumberGenerator> ManagedInterpreter<R> {
    pub const DEFAULT_OPERATION_DURATION: Duration = Duration::from_millis(2);
    pub const DEFAULT_DELAY_TICK_DURATION: Duration = Duration::from_nanos(16666667);
    pub const DEFAULT_SOUND_TICK_DURATION: Duration = Duration::from_nanos(16666667);

    pub fn new(image: impl Image, rand: R) -> Self {
        Self::new_with_durations(
            image,
            rand,
            Self::DEFAULT_OPERATION_DURATION,
            Self::DEFAULT_DELAY_TICK_DURATION,
            Self::DEFAULT_SOUND_TICK_DURATION,
        )
    }

    pub fn new_with_durations(
        image: impl Image,
        rand: R,
        operation_duration: Duration,
        delay_tick_duration: Duration,
        sound_tick_duration: Duration,
    ) -> Self {
        Self {
            inner: Interpreter::new(image, ManagedPlatform::new(rand)),
            operation_duration,
            delay_tick_duration,
            sound_tick_duration,
            delay_accumulated: Duration::from_secs(0),
            sound_accumulated: Duration::from_secs(0),
        }
    }

    pub fn simulate_one_instruction(&mut self) -> Result<()> {
        self.inner.run_next_instruction()?;

        self.delay_accumulated += self.operation_duration;
        self.sound_accumulated += self.operation_duration;

        self.decrease_delay_accumulated();
        self.decrease_sound_accumulated();

        Ok(())
    }

    pub fn simulate_duration(&mut self, duration: Duration) -> Result<()> {
        let target_instructions = (duration.as_nanos() / self.operation_duration.as_nanos()) as u64;

        for _ in 0..target_instructions {
            self.simulate_one_instruction()?;
        }

        let remainder = duration.as_nanos() % self.operation_duration.as_nanos();
        if remainder > 0 {
            self.delay_accumulated += Duration::from_nanos(remainder as u64);
            self.sound_accumulated += Duration::from_nanos(remainder as u64);

            self.decrease_delay_accumulated();
            self.decrease_sound_accumulated();
        }

        Ok(())
    }

    fn decrease_sound_accumulated(&mut self) {
        while self.sound_accumulated >= self.sound_tick_duration {
            let current = self.inner.platform().sound_timer;
            if current > 0 {
                self.inner.platform_mut().sound_timer = current - 1;
            }
            self.sound_accumulated -= self.sound_tick_duration;
        }
    }

    fn decrease_delay_accumulated(&mut self) {
        while self.delay_accumulated >= self.delay_tick_duration {
            let current = self.inner.platform().delay_timer;
            if current > 0 {
                self.inner.platform_mut().delay_timer = current - 1;
            }
            self.delay_accumulated -= self.delay_tick_duration;
        }
    }

    pub fn inner(&self) -> &Interpreter<ManagedPlatform<R>> {
        &self.inner
    }

    pub fn frame_buffer(&self) -> &FrameBuffer {
        &self.inner.platform().frame_buffer
    }

    pub fn set_key_down(&mut self, key: Key, is_down: bool) {
        let platform = self.inner.platform_mut();
        platform.key_state[key.as_usize()] = is_down;

        if is_down {
            platform.key_press_queue = Some(key);
        }
    }
}

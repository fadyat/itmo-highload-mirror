use std::fmt::Write as _;

use chip8::DebugTarget;
use derive_more::From;
use ratatui::{
    prelude::*,
    widgets::{Block, Clear, WidgetRef, block},
};

use crate::{
    keys,
    runner::{Keyboard, Runner},
};

const SMALL_SCREEN_SIZE_MSG: &str = "terminal screen size too small";

impl<R: chip8::RandomNumberGenerator> Widget for &Runner<R> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        let [main_area, controls_area] =
            Layout::vertical([Constraint::Fill(1), Constraint::Length(1)]).areas(area);
        if self.fullscreen {
            Fullscreen::from(self).render(main_area, buf);
        } else {
            DebugWidgets::from(self).render(main_area, buf);
        }
        if self.show_help {
            let help_area = centered(main_area, 55, 20).expect(SMALL_SCREEN_SIZE_MSG);
            HelpPopup.render(help_area, buf);
        }
        Controls.render(controls_area, buf);
    }
}

struct Fullscreen<'a> {
    frame_buffer: &'a chip8::FrameBuffer,
    error: &'a Option<chip8::Error>,
}

impl Fullscreen<'_> {
    const WIDTH: u16 = chip8::SCREEN_WIDTH as u16 * 2 + 2;
    const HEIGHT: u16 = chip8::SCREEN_HEIGHT as u16 + 2;
}

impl<'a, R: chip8::RandomNumberGenerator> From<&'a Runner<R>> for Fullscreen<'a> {
    fn from(value: &'a Runner<R>) -> Self {
        Self {
            frame_buffer: value.interpreter.frame_buffer(),
            error: &value.error,
        }
    }
}

impl Widget for Fullscreen<'_> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        let Some(area) = centered(area, Self::WIDTH, Self::HEIGHT) else {
            Text::from(SMALL_SCREEN_SIZE_MSG)
                .centered()
                .render(area, buf);
            return;
        };

        let mut block = Block::bordered();
        if let Some(err) = self.error {
            block = block.border_style(Color::Red);
            block = block.title_bottom(Line::from(err.to_string()).centered());
        }

        block.render_ref(area, buf);
        let area = block.inner(area);
        for (y, row) in self.frame_buffer.iter_rows().enumerate() {
            for (x, &pixel) in row.iter().enumerate() {
                let pos = (area.x + x as u16 * 2, area.y + y as u16);
                if let Some(cell) = buf.cell_mut(pos) {
                    cell.set_symbol(if pixel { "██" } else { "  " });
                }
            }
        }
    }
}

struct DebugWidgets<'a> {
    screen: Screen<'a>,
    keypad: &'a Keyboard,
    registers: Registers<'a>,
    sprite: Sprite<'a>,
}

impl<'a, R: chip8::RandomNumberGenerator> From<&'a Runner<R>> for DebugWidgets<'a> {
    fn from(value: &'a Runner<R>) -> Self {
        let inner = value.interpreter.inner();
        Self {
            screen: value.interpreter.frame_buffer().into(),
            keypad: &value.keyboard,
            registers: Registers {
                main: inner.registers(),
                index: inner.index_register(),
                program_counter: inner.program_counter(),
                opcode: inner.opcode(),
            },
            sprite: inner.current_sprite().into(),
        }
    }
}

impl<'a> Widget for DebugWidgets<'a> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        let [left, right] = Layout::horizontal([
            Constraint::Length(chip8::SCREEN_WIDTH as u16 + 2),
            Constraint::Fill(1),
        ])
        .areas(area);

        let [screen_area, under_screen] = Layout::vertical([
            Constraint::Length(chip8::SCREEN_HEIGHT as u16 / 2 + 2),
            Constraint::Fill(1),
        ])
        .areas(left);
        let [sprite_area, keypad_area] =
            Layout::horizontal([Constraint::Length(10), Constraint::Fill(1)]).areas(under_screen);

        render_bordered("Screen", self.screen, screen_area, buf);
        render_bordered("Registers", self.registers, right, buf);
        render_bordered("Sprite", self.sprite, sprite_area, buf);
        render_bordered("Keypad", self.keypad, keypad_area, buf);
    }
}

#[derive(From)]
struct Screen<'a>(&'a chip8::FrameBuffer);

impl Widget for Screen<'_> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        if area.width < chip8::SCREEN_WIDTH as u16
            || area.height < (chip8::SCREEN_HEIGHT / 2) as u16
        {
            Text::from("not enough space").centered().render(area, buf);
            return;
        }

        let fb = self.0.as_ref();
        for y in 0..(chip8::SCREEN_HEIGHT / 2) {
            let top_row = fb[2 * y].into_iter();
            let down_row = fb[2 * y + 1].into_iter();
            for (x, (top, down)) in top_row.zip(down_row).enumerate() {
                let char = match (top, down) {
                    (true, true) => '█',
                    (true, false) => '▀',
                    (false, true) => '▄',
                    (false, false) => ' ',
                };

                buf.cell_mut((area.x + x as u16, area.y + y as u16))
                    .unwrap()
                    .set_char(char);
            }
        }
    }
}

struct Registers<'a> {
    main: &'a [chip8::Word],
    index: chip8::Address,
    program_counter: chip8::Address,
    opcode: chip8::OpCode,
}

impl Widget for Registers<'_> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        let main: Line = if self.main.is_empty() {
            "<no registers>".into()
        } else {
            let mut str = String::with_capacity(5 * self.main.len());
            for word in self.main {
                write!(&mut str, "{word:#04X} ").unwrap();
            }
            str.into()
        };

        let index = Line::from(format!("Index: {:#X}", self.index.as_usize()));
        let program_cnt = Line::from(format!(
            "Program counter: {:#X}",
            self.program_counter.as_usize()
        ));

        let mut opcode = format!("Opcode: {:#08X} - ", self.opcode.as_u16());
        match chip8::Operation::try_from(self.opcode) {
            Ok(op) => write!(opcode, "{op:?}"),
            Err(_) => write!(opcode, "<unrecognised op>"),
        }
        .unwrap();
        let opcode = Line::from(opcode);

        let lines = Text::from(vec![main, index, program_cnt, opcode]);
        lines.render(area, buf);
    }
}

#[derive(From)]
struct Sprite<'a>(chip8::Sprite<'a>);

impl Widget for Sprite<'_> {
    fn render(self, area: Rect, buf: &mut Buffer) {
        for chip8::Point { x, y } in self.0.iter_pixels() {
            let upper = y % 2 == 0;
            if let Some(c) = buf.cell_mut((area.x + x as u16, area.y + y as u16 / 2)) {
                let new_char = match (c.symbol(), upper) {
                    ("▄", true) => '█',
                    (_, true) => '▀',
                    ("▀", false) => '█',
                    (_, false) => '▄',
                };
                c.set_char(new_char);
            }
        }
    }
}

impl Widget for &Keyboard {
    fn render(self, area: Rect, buf: &mut Buffer) {
        const WIDHT: usize = 4;
        for (y, row) in self.0.chunks_exact(WIDHT).enumerate() {
            for (x, &is_pressed) in row.iter().enumerate() {
                let index = y * WIDHT + x;
                let symbol = keys::SYMBOLS[index];
                let symbol_x = area.x + x as u16 * 3;
                let symbol_y = area.y + y as u16;
                if is_pressed {
                    buf.set_string(symbol_x, symbol_y, "[ ]", Style::default());
                }
                buf.set_string(symbol_x + 1, symbol_y, symbol, Style::default());
            }
        }
    }
}

fn render_bordered<'a>(
    title: impl Into<block::Title<'a>>,
    widget: impl Widget,
    area: Rect,
    buf: &mut Buffer,
) {
    let block = Block::bordered().title(title);
    widget.render(block.inner(area), buf);
    block.render(area, buf);
}

fn centered(area: Rect, width: u16, height: u16) -> Option<Rect> {
    if area.width < width || area.height < height {
        return None;
    }
    let offset_x = (area.width - width) / 2;
    let offset_y = (area.height - height) / 2;
    Some(Rect {
        x: area.x + offset_x,
        y: area.y + offset_y,
        width,
        height,
    })
}

#[derive(Clone, Copy)]
struct HelpPopup;

impl Widget for HelpPopup {
    fn render(self, area: Rect, buf: &mut Buffer) {
        Clear.render(area, buf);
        let text = Text::from(vec![
            "(1234..zxcv) - chip8 keyboard".into(),
            "(p) - play/pause".into(),
            "(→) or (n) - next instruction".into(),
            "(l) - fullscreen/debug mode".into(),
            "(k) - toggle sticky keyboard".into(),
            "(t) - take snapshot".into(),
            "(y) - restore previous snapshot".into(),
            "(h) - show/hide this help".into(),
            "(Esc) - exit".into(),
        ])
        .centered();
        render_bordered("Help", text, area, buf);
    }
}

#[derive(Clone, Copy)]
struct Controls;

impl Widget for Controls {
    fn render(self, area: Rect, buf: &mut Buffer) {
        buf.set_style(area, Style::new().on_dark_gray());

        Line::from("(1234..zxcv) keyboard | (h) help | (Esc) exit")
            .centered()
            .render(area, buf);
    }
}

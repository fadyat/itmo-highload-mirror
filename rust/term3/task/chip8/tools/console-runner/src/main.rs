mod keys;
mod runner;

use std::{
    fs,
    path::PathBuf,
    time::{self, Duration},
};

use anyhow::Result;
use chip8::RandomNumberGenerator;
use clap::Parser;
use rand::{Rng, SeedableRng, rngs::SmallRng};
use ratatui::{
    Terminal,
    crossterm::event::{self, Event, KeyEventKind},
};

use crate::runner::Runner;

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Args {
    image: PathBuf,

    #[arg(short, long)]
    debug: bool,
}

fn main() -> Result<()> {
    let args = Args::parse();

    let image_data = fs::read(args.image)?;
    let image = chip8::Ch8Image::new(image_data)?;

    let rng = SmallRng::from_seed(rand::random());
    let interpreter = chip8::ManagedInterpreter::new(image, MyRng(rng));
    let debugger = Runner::new(interpreter, args.debug);
    let terminal = ratatui::init();
    run_debugger(debugger, terminal)?;
    ratatui::restore();
    Ok(())
}

fn run_debugger<R, B>(mut debugger: Runner<R>, mut terminal: Terminal<B>) -> Result<()>
where
    R: chip8::RandomNumberGenerator + Clone,
    B: ratatui::backend::Backend,
{
    let mut last_tick = time::Instant::now();
    loop {
        terminal.draw(|f| f.render_widget(&debugger, f.area()))?;

        while event::poll(Duration::ZERO)? {
            if let Event::Key(e) = event::read()? {
                if e.kind != KeyEventKind::Press {
                    continue;
                }
                if debugger.handle(e) {
                    return Ok(());
                }
            }
        }

        let now = time::Instant::now();
        let elapsed = now.saturating_duration_since(last_tick);
        last_tick = now;
        debugger.tick(elapsed);
    }
}

#[derive(Clone)]
struct MyRng(SmallRng);

impl RandomNumberGenerator for MyRng {
    fn get_random_word(&mut self) -> chip8::Word {
        self.0.random()
    }
}

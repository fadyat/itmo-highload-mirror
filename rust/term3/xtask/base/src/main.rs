use xtask_base::{Command, run_command};

use anyhow::Result;
use clap::Parser;

////////////////////////////////////////////////////////////////////////////////

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[command(subcommand)]
    cmd: Command,
}

fn main() -> Result<()> {
    let args = Args::parse();
    run_command(args.cmd)
}

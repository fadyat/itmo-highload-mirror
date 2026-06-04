use std::{
    collections::HashSet,
    fs::File,
    io::{BufRead, BufReader},
};

fn main() {
    let args = std::env::args().skip(1).collect::<Vec<String>>();
    if args.len() != 2 {
        eprintln!("Usage: cargo run <file1> <file2>");
        return;
    }

    let l1 = BufReader::new(File::open(&args[0]).unwrap())
        .lines()
        .map_while(Result::ok)
        .collect::<HashSet<String>>();

    let l2 = BufReader::new(File::open(&args[1]).unwrap())
        .lines()
        .map_while(Result::ok)
        .collect::<HashSet<String>>();

    for a in l1.intersection(&l2) {
        println!("{a}");
    }
}

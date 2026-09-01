#![forbid(unsafe_code)]

use rayon::prelude::*;
use std::fs;
use std::{
    fs::File,
    io::{self, BufRead, BufReader},
    path::{Path, PathBuf},
};

#[derive(Debug, PartialEq, Eq)]
pub struct Match {
    pub path: PathBuf,
    pub line: String,
    pub line_number: usize,
}

#[derive(Debug)]
pub struct Error {
    pub path: PathBuf,
    pub error: io::Error,
}

pub enum Event {
    Match(Match),
    Error(Error),
}

fn collect_files(start: PathBuf) -> Vec<PathBuf> {
    let mut files = Vec::new();
    let mut stack = vec![start];

    while let Some(p) = stack.pop() {
        if let Ok(metadata) = fs::metadata(&p) {
            if metadata.is_file() {
                files.push(p);
            } else if metadata.is_dir()
                && let Ok(rd) = p.read_dir()
            {
                stack.extend(rd.filter_map(Result::ok).map(|e| e.path()));
            }
        }
    }

    files
}

pub fn run<P: AsRef<Path>>(path: P, pattern: &str) -> Vec<Event> {
    let start = path.as_ref().to_path_buf();

    if let Err(e) = start.metadata() {
        return vec![Event::Error(Error {
            path: start,
            error: e,
        })];
    }

    let files = collect_files(start);

    files
        .par_iter()
        .flat_map_iter(|fp| {
            let mut out = Vec::new();
            match File::open(fp) {
                Ok(f) => {
                    let reader = BufReader::new(f);
                    for (i, line_res) in reader.lines().enumerate() {
                        match line_res {
                            Ok(line) => {
                                if line.contains(pattern) {
                                    out.push(Event::Match(Match {
                                        path: fp.clone(),
                                        line,
                                        line_number: i + 1,
                                    }));
                                }
                            }
                            Err(e) => {
                                out.push(Event::Error(Error {
                                    path: fp.clone(),
                                    error: e,
                                }));
                                break;
                            }
                        }
                    }
                }
                Err(e) => out.push(Event::Error(Error {
                    path: fp.clone(),
                    error: e,
                })),
            }
            out.into_iter()
        })
        .collect()
}

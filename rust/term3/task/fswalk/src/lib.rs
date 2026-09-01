#![forbid(unsafe_code)]

use std::{fs, io, path::Path};

type Callback<'a> = dyn FnMut(&mut Handle) + 'a;

#[derive(Default)]
pub struct Walker<'a> {
    callbacks: Vec<Box<Callback<'a>>>,
}

impl<'a> Walker<'a> {
    pub fn new() -> Self {
        Self { callbacks: vec![] }
    }

    pub fn add_callback<F>(&mut self, callback: F)
    where
        F: FnMut(&mut Handle) + 'a,
    {
        self.callbacks.push(Box::new(callback));
    }

    pub fn walk<P>(&mut self, path: P) -> io::Result<()>
    where
        P: AsRef<Path>,
    {
        if self.callbacks.is_empty() {
            return Ok(());
        }

        walk_inner(path.as_ref(), &mut self.callbacks)
    }
}

fn partition_by<'a, F>(callbacks: &mut [Box<Callback<'a>>], mut f: F) -> usize
where
    F: FnMut(&mut Box<Callback<'a>>) -> bool,
{
    let mut write = 0usize;
    for i in 0..callbacks.len() {
        if f(&mut callbacks[i]) {
            callbacks.swap(write, i);
            write += 1;
        }
    }
    write
}

fn walk_inner<'a>(path: &Path, callbacks: &mut [Box<Callback<'a>>]) -> io::Result<()> {
    let entries = fs::read_dir(path)?;
    let mut error: Option<io::Error> = None;

    for entry in entries {
        if let Err(e) = handle_entry(entry, callbacks) {
            error.get_or_insert(e);
        }
    }

    error.map_or(Ok(()), Err)
}

fn handle_entry<'a>(
    entry: Result<fs::DirEntry, io::Error>,
    callbacks: &mut [Box<Callback<'a>>],
) -> io::Result<()> {
    let entry = entry?;
    let path = entry.path();
    let metadata = entry.metadata()?;

    if metadata.is_dir() {
        let write = partition_by(callbacks, |cb| {
            let mut h = Handle::Dir(DirHandle {
                path: &path,
                descend: false,
            });
            cb(&mut h);
            matches!(&h, Handle::Dir(d) if d.descend)
        });

        if write > 0 {
            walk_inner(&path, &mut callbacks[..write])?;
        }
    } else if metadata.is_file() {
        let write = partition_by(callbacks, |cb| {
            let mut h = Handle::File(FileHandle {
                path: &path,
                read: false,
            });
            cb(&mut h);
            matches!(&h, Handle::File(f) if f.read)
        });

        if write > 0 {
            let content_vec = fs::read(&path)?;
            for cb in &mut callbacks[..write] {
                cb(&mut Handle::Content {
                    file_path: &path,
                    content: &content_vec,
                });
            }
        }
    }

    Ok(())
}

pub enum Handle<'a> {
    Dir(DirHandle<'a>),
    File(FileHandle<'a>),
    Content {
        file_path: &'a Path,
        content: &'a [u8],
    },
}

pub struct DirHandle<'a> {
    path: &'a Path,
    descend: bool,
}

impl<'a> DirHandle<'a> {
    pub fn descend(&mut self) {
        self.descend = true
    }

    pub fn path(&self) -> &Path {
        self.path
    }
}

pub struct FileHandle<'a> {
    path: &'a Path,
    read: bool,
}

impl<'a> FileHandle<'a> {
    pub fn read(&mut self) {
        self.read = true;
    }

    pub fn path(&self) -> &Path {
        self.path
    }
}

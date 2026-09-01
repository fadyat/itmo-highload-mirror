#![forbid(unsafe_code)]

use std::io::copy;
use std::net::{TcpListener, TcpStream};
use std::result::Result;
use std::thread;

pub fn run_proxy(port: u32, destination: String) {
    _run_proxy(port, destination).unwrap();
}

pub fn _run_proxy(port: u32, destination: String) -> Result<(), std::io::Error> {
    let listener = TcpListener::bind(format!("127.0.0.1:{}", port))?;

    for incoming in listener.incoming() {
        let mut client = incoming?;
        let mut server = TcpStream::connect(&destination)?;
        let mut client_clone = client.try_clone()?;
        let mut server_clone = server.try_clone()?;

        thread::spawn(move || copy(&mut client, &mut server).unwrap());
        thread::spawn(move || copy(&mut server_clone, &mut client_clone).unwrap());
    }

    Ok(())
}

use std::{
    io::{Read, Write},
    net::TcpStream,
    sync::{Arc, Mutex},
};

use anyhow::Result;
use clap::Parser;
use openssl::ssl::{SslConnector, SslConnectorBuilder, SslMethod, SslRef, SslVersion};

#[derive(Parser, Debug)]
struct Args {
    #[arg(
        long,
        help = "Address to connect to, e.g. 'github.com:443' or 'localhost:8080'"
    )]
    address: String,

    #[arg(long, help = "Request document path, e.g. '/index.html'")]
    path: String,

    #[arg(
        long,
        value_enum,
        help = "TLS version to use",
        value_parser = |s: &str| -> Result<SslVersion, String> {
            match s {
                "v1.2" => Ok(SslVersion::TLS1_2),
                "v1.3" => Ok(SslVersion::TLS1_3),
                _ => Err(format!("Invalid TLS version: {}", s)),
            }
        },
        default_value = "v1.3"
    )]
    tls_version: SslVersion,

    #[arg(
        long,
        help = "Ciphersuites to use concatented with ':', e.g. 'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384'"
    )]
    ciphers: String,

    #[arg(
        long,
        help = "Enable key logging, based on SSLKEYLOGFILE env var",
    )]
    with_key_log: bool,
}

impl Args {
    fn host(&self) -> String {
        self.address
            .split(':')
            .next()
            .unwrap_or(&self.address)
            .to_string()
    }
}

fn new_ssl_connector(args: &Args) -> Result<SslConnector> {
    let mut b = SslConnector::builder(SslMethod::tls_client())?;

    b.set_min_proto_version(Some(args.tls_version))?;
    b.set_max_proto_version(Some(args.tls_version))?;
    if args.tls_version == SslVersion::TLS1_2 {
        b.set_cipher_list(&args.ciphers)?;
    } else {
        b.set_ciphersuites(&args.ciphers)?;
    }
    b.set_verify(openssl::ssl::SslVerifyMode::PEER);

    if args.with_key_log {
        set_key_log_file(&mut b)?;
    }

    Ok(b.build())
}

fn set_key_log_file(b: &mut SslConnectorBuilder) -> Result<()> {
    let key_log_file = std::env::var("SSLKEYLOGFILE").map_err(|_| {
        std::io::Error::new(
            std::io::ErrorKind::NotFound,
            "SSLKEYLOGFILE environment variable not set",
        )
    })?;

    let file = std::fs::OpenOptions::new()
        .create(true)
        .truncate(true)
        .write(true)
        .open(key_log_file)?;

    let file = Arc::new(Mutex::new(file));
    b.set_keylog_callback({
        let file = Arc::clone(&file);
        move |_: &SslRef, line: &str| {
            let mut f = file.lock().unwrap();
            writeln!(f, "{line}").unwrap();
        }
    });

    Ok(())
}

fn main() -> Result<()> {
    let args = Args::parse();

    let ssl_connector = new_ssl_connector(&args)?;
    let tcp_stream = TcpStream::connect(args.address.clone())?;
    let mut ssl_stream = ssl_connector.connect(&args.host(), tcp_stream)?;

    ssl_stream.write_all(
        format!(
            "GET {} HTTP/1.1\r\nHost: {}\r\nConnection: close\r\n\r\n",
            args.path,
            args.host()
        )
        .as_bytes(),
    )?;

    let mut response = Vec::new();
    ssl_stream.read_to_end(&mut response)?;
    println!("Response: {}", String::from_utf8_lossy(&response));

    ssl_stream.shutdown()?;

    Ok(())
}

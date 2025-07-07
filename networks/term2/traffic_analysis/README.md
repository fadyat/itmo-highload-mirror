## Requirements

Here we are using [`rust-openssl`](https://github.com/sfackler/rust-openssl) library to implement a TLS client.
This library is a Rust wrapper around OpenSSL, which provides a high-level API for working with TLS.

## How to run

```bash
cargo build --release
cp target/release/traffic_analysis .
```

```txt
Usage: traffic_analysis [OPTIONS] --address <ADDRESS> --path <PATH> --ciphers <CIPHERS>

Options:
      --address <ADDRESS>          Address to connect to, e.g. 'github.com:443' or 'localhost:8080'
      --path <PATH>                Request document path, e.g. '/index.html'
      --tls-version <TLS_VERSION>  TLS version to use [default: v1.3]
      --ciphers <CIPHERS>          Ciphersuites to use concatented with ':', e.g. 'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384'
      --with-key-log               Enable key logging, based on SSLKEYLOGFILE env var
  -h, --help                       Print help
```

## Results

Keys and wireshark logs can be found at [.out](./out) directory.
Traffic can be analyzed using [Wireshark](https://www.wireshark.org/) with [setting up path to key log file](https://wiki.wireshark.org/TLS#using-the-pre-master-secret).

### TLS v1.2

```bash
SSLKEYLOGFILE=./.out/key_log_v12.txt ./traffic_analysis \
    --address github.com:443 \
    --path /fadyat \
    --tls-version v1.2 \
    --ciphers TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256:TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 \
    --with-key-log
```

### TLS v1.3

```bash
SSLKEYLOGFILE=./.out/key_log_v13.txt ./traffic_analysis \
    --address github.com:443 \
    --path /fadyat \
    --tls-version v1.3 \
    --ciphers TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384 \
    --with-key-log
```

#![forbid(unsafe_code)]

mod runtime;
mod scheduler;
mod timer;

#[cfg(feature = "net")]
mod network;

////////////////////////////////////////////////////////////////////////////////

pub use rio_macros::test;

pub use runtime::{Runtime, runtime_id, spawn};
pub use timer::sleep;

#[cfg(feature = "net")]
pub use network::UdpSocket;

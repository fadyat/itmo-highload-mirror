#![forbid(unsafe_code)]

use std::{cell::RefCell, collections::VecDeque, fmt::Debug, rc::Rc};

use thiserror::Error;

#[derive(Error, Debug)]
#[error("channel is closed")]
pub struct SendError<T: Debug> {
    pub value: T,
}

struct Inner<T> {
    queue: VecDeque<T>,
    closed: bool,
    senders: usize,
    receiver_exists: bool,
}

pub struct Sender<T> {
    inner: Rc<RefCell<Inner<T>>>,
}

impl<T: Debug> Sender<T> {
    pub fn send(&self, value: T) -> Result<(), SendError<T>> {
        let mut inner = self.inner.borrow_mut();
        if inner.closed || !inner.receiver_exists {
            return Err(SendError { value });
        }

        inner.queue.push_back(value);
        Ok(())
    }

    pub fn is_closed(&self) -> bool {
        let inner = self.inner.borrow();
        inner.closed || !inner.receiver_exists
    }

    pub fn same_channel(&self, other: &Self) -> bool {
        Rc::ptr_eq(&self.inner, &other.inner)
    }
}

impl<T> Clone for Sender<T> {
    fn clone(&self) -> Self {
        {
            self.inner.borrow_mut().senders += 1;
        }

        Sender {
            inner: Rc::clone(&self.inner),
        }
    }
}

impl<T> Drop for Sender<T> {
    fn drop(&mut self) {
        let mut inner = self.inner.borrow_mut();
        if inner.senders > 0 {
            inner.senders -= 1;
            if inner.senders == 0 {
                inner.closed = true;
            }
        }
    }
}

#[derive(Error, Debug)]
pub enum ReceiveError {
    #[error("channel is empty")]
    Empty,
    #[error("channel is closed")]
    Closed,
}

pub struct Receiver<T> {
    inner: Rc<RefCell<Inner<T>>>,
}

impl<T> Receiver<T> {
    pub fn recv(&mut self) -> Result<T, ReceiveError> {
        let mut inner = self.inner.borrow_mut();
        if let Some(x) = inner.queue.pop_front() {
            return Ok(x);
        }

        if inner.closed || inner.senders == 0 {
            return Err(ReceiveError::Closed);
        }

        Err(ReceiveError::Empty)
    }

    pub fn close(&mut self) {
        self.inner.borrow_mut().closed = true;
    }
}

impl<T> Drop for Receiver<T> {
    fn drop(&mut self) {
        let mut inner = self.inner.borrow_mut();
        inner.closed = true;
        inner.receiver_exists = false;
    }
}

pub fn channel<T>() -> (Sender<T>, Receiver<T>) {
    let inner = Inner {
        queue: VecDeque::new(),
        closed: false,
        senders: 1,
        receiver_exists: true,
    };

    let rc = Rc::new(RefCell::new(inner));

    (
        Sender {
            inner: Rc::clone(&rc),
        },
        Receiver { inner: rc },
    )
}

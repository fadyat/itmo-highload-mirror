#![forbid(unsafe_code)]

use std::rc::Rc;

struct Node<T> {
    value: T,
    prev: Option<Rc<Node<T>>>,
}

pub struct PStack<T> {
    head: Option<Rc<Node<T>>>,
    size: usize,
}

impl<T> Default for PStack<T> {
    fn default() -> Self {
        PStack {
            head: None,
            size: 0,
        }
    }
}

impl<T> Clone for PStack<T> {
    fn clone(&self) -> Self {
        PStack {
            head: self.head.clone(),
            size: self.size,
        }
    }
}

impl<T> PStack<T> {
    pub fn new() -> Self {
        Self {
            head: None,
            size: 0,
        }
    }

    pub fn push(&self, value: T) -> Self {
        let new_head = Rc::new(Node {
            value,
            prev: self.head.clone(),
        });

        PStack {
            head: Some(new_head),
            size: self.len() + 1,
        }
    }

    pub fn pop(&self) -> Option<(&T, Self)> {
        self.head.as_ref().map(|node| {
            (
                &node.value,
                PStack {
                    head: node.prev.clone(),
                    size: self.len() - 1,
                },
            )
        })
    }

    pub fn len(&self) -> usize {
        self.size
    }

    pub fn is_empty(&self) -> bool {
        self.size == 0
    }

    pub fn iter(&self) -> impl Iterator<Item = &T> {
        std::iter::successors(self.head.as_ref(), |node_rc| node_rc.prev.as_ref())
            .map(|node_rc| &node_rc.value)
    }
}

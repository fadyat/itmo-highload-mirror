#![forbid(unsafe_code)]

pub use gc_derive::Scan;

use std::{
    cell::RefCell,
    collections::{HashMap, HashSet},
    marker::PhantomData,
    ops::Deref,
    rc::{Rc, Weak},
};

/// Gc is a garbage-collected pointer to an object of type T, managed by an Arena.
pub struct Gc<T> {
    weak: Weak<T>,
}

impl<T> Clone for Gc<T> {
    fn clone(&self) -> Self {
        Self {
            weak: self.weak.clone(),
        }
    }
}

impl<T> Gc<T> {
    pub fn borrow(&self) -> GcRef<'_, T> {
        GcRef {
            rc: self.weak.upgrade().expect("Tried to borrow a dropped Gc"),
            lifetime: PhantomData,
        }
    }
}

/// GcRef is a reference to the object managed by Gc, with a lifetime tied to the Gc.
pub struct GcRef<'a, T> {
    rc: Rc<T>,
    lifetime: PhantomData<&'a Gc<T>>,
}

impl<'a, T> Deref for GcRef<'a, T> {
    type Target = T;

    fn deref(&self) -> &Self::Target {
        &self.rc
    }
}

pub trait Scan {
    /// Scan the object for Gc pointers and apply the function `f` to each found pointer.
    /// This is used by the garbage collector to find reachable objects.
    /// Callers should implement this method to traverse their fields and call `f` on any Gc pointers they contain.
    fn scan(&self, f: &mut dyn FnMut(usize));
}

impl<T> Scan for Gc<T> {
    fn scan(&self, f: &mut dyn FnMut(usize)) {
        f(self.weak.as_ptr() as usize);
    }
}

impl<T: Scan> Scan for Option<T> {
    fn scan(&self, f: &mut dyn FnMut(usize)) {
        if let Some(inner) = self {
            inner.scan(f);
        }
    }
}

impl<T: Scan> Scan for Vec<T> {
    fn scan(&self, f: &mut dyn FnMut(usize)) {
        for item in self {
            item.scan(f);
        }
    }
}

impl<T: Scan> Scan for RefCell<T> {
    fn scan(&self, f: &mut dyn FnMut(usize)) {
        self.borrow().scan(f);
    }
}

impl Scan for i32 {
    fn scan(&self, _f: &mut dyn FnMut(usize)) {}
}

/// Arena is a garbage-collected memory pool that manages the lifetime of objects allocated within it.
/// It supports allocation of objects, tracking of reachable objects, and garbage collection via mark-and-sweep.
#[derive(Default)]
pub struct Arena {
    objects: HashMap<usize, Rc<dyn Scan>>,
}

impl Arena {
    pub fn new() -> Self {
        Self {
            objects: HashMap::new(),
        }
    }

    pub fn allocation_count(&self) -> usize {
        self.objects.len()
    }

    pub fn alloc<T: Scan + 'static>(&mut self, obj: T) -> Gc<T> {
        let rc = Rc::new(obj);
        let weak = Rc::downgrade(&rc);
        let addr = Rc::as_ptr(&rc) as usize;

        // storing as Rc<dyn Scan> to allow heterogeneous collections
        let rc_dyn: Rc<dyn Scan> = rc.clone();
        self.objects.insert(addr, rc_dyn);

        Gc { weak }
    }

    /// Performs a mark-and-sweep garbage collection to reclaim unreachable objects.
    pub fn sweep(&mut self) {
        let mut incoming: HashMap<usize, usize> = self.objects.keys().map(|&k| (k, 0)).collect();

        for rc in self.objects.values() {
            rc.scan(&mut |child_addr: usize| {
                if let Some(cnt) = incoming.get_mut(&child_addr) {
                    *cnt += 1;
                }
            });
        }

        let roots: Vec<usize> = self
            .objects
            .iter()
            .filter_map(|(&addr, rc)| {
                if Rc::weak_count(rc) > *incoming.get(&addr).unwrap_or(&0) {
                    Some(addr)
                } else {
                    None
                }
            })
            .collect();

        let mut marked: HashSet<usize> = HashSet::new();
        for root in roots.into_iter() {
            if !marked.contains(&root) {
                self.mark_all(root, &mut marked);
            }
        }

        self.objects.retain(|addr, _| marked.contains(addr));
    }

    /// Recursively marks all reachable objects starting from the given root address.
    /// This function updates the `marked` set with the addresses of all reachable objects.
    fn mark_all(&self, root_addr: usize, marked: &mut HashSet<usize>) {
        let mut stack = vec![root_addr];

        while let Some(addr) = stack.pop() {
            if !marked.insert(addr) {
                continue;
            }

            if let Some(rc) = self.objects.get(&addr) {
                rc.scan(&mut |child_addr: usize| {
                    if !marked.contains(&child_addr) && self.objects.contains_key(&child_addr) {
                        stack.push(child_addr);
                    }
                });
            }
        }
    }
}

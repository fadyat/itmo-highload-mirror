use crate::{
    data::ObjectId,
    error::*,
    object::{Object, Store},
    storage::StorageTransaction,
};

use std::{
    any::Any,
    cell::{Ref, RefCell, RefMut},
    collections::HashMap,
    marker::PhantomData,
    rc::Rc,
};

////////////////////////////////////////////////////////////////////////////////

pub struct Transaction<'a> {
    inner: Box<dyn StorageTransaction + 'a>,
    objects: RefCell<HashMap<ObjectId, TransactionObject>>,
}

impl<'a> Transaction<'a> {
    pub(crate) fn new(inner: Box<dyn StorageTransaction + 'a>) -> Self {
        Self {
            inner,
            objects: RefCell::new(HashMap::new()),
        }
    }

    pub fn create<T: Object>(&self, obj: T) -> Result<Tx<'_, T>> {
        if !self.inner.table_exists(T::SCHEMA.table_name)? {
            self.inner.create_table(&T::SCHEMA)?;
        }
        let row = obj.to_row();
        let id = self.inner.insert_row(&T::SCHEMA, &row)?;
        let rc = Rc::new(RefCell::new((
            ObjectState::Clean,
            Box::new(obj) as Box<dyn Store>,
        )));
        self.objects.borrow_mut().insert(id, rc.clone());
        Ok(Tx {
            inner: rc,
            id,
            object_lifetime: PhantomData,
        })
    }

    fn construct_not_found_error(id: ObjectId, type_name: &'static str) -> Error {
        Error::NotFound(Box::new(NotFoundError {
            object_id: id,
            type_name,
        }))
    }

    pub fn get<T: Object>(&self, id: ObjectId) -> Result<Tx<'_, T>> {
        if let Some(obj_rc) = self.objects.borrow().get(&id) {
            if obj_rc.borrow().0 == ObjectState::Removed {
                return Err(Self::construct_not_found_error(id, T::SCHEMA.type_name));
            }
            return Ok(Tx {
                inner: obj_rc.clone(),
                id,
                object_lifetime: PhantomData,
            });
        }

        if !self.inner.table_exists(T::SCHEMA.table_name)? {
            return Err(Self::construct_not_found_error(id, T::SCHEMA.type_name));
        }

        let row = self.inner.select_row(id, &T::SCHEMA)?;
        let store: Box<dyn Store> = Box::new(T::from_row(&row));
        let rc = Rc::new(RefCell::new((ObjectState::Clean, store)));
        self.objects.borrow_mut().insert(id, rc.clone());
        Ok(Tx {
            inner: rc,
            id,
            object_lifetime: PhantomData,
        })
    }

    pub fn commit(self) -> Result<()> {
        // borrow the map once and iterate; borrow each object briefly inside loop
        for (id, obj_rc) in self.objects.borrow().iter() {
            let b = obj_rc.borrow();
            match b.0 {
                ObjectState::Clean => {}
                ObjectState::Modified => {
                    let _ = self.inner.update_row(*id, b.1.get_schema(), &b.1.get_row());
                }
                ObjectState::Removed => {
                    let _ = self.inner.delete_row(*id, b.1.get_schema());
                }
            }
        }
        self.inner.commit()
    }

    pub fn rollback(self) -> Result<()> {
        self.inner.rollback()
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum ObjectState {
    Clean,
    Modified,
    Removed,
}

type TransactionObject = Rc<RefCell<(ObjectState, Box<dyn Store>)>>;

#[derive(Clone)]
pub struct Tx<'a, T> {
    inner: TransactionObject,
    id: ObjectId,
    object_lifetime: PhantomData<&'a T>,
}

impl<'a, T: Any> Tx<'a, T> {
    pub fn id(&self) -> ObjectId {
        self.id
    }

    pub fn state(&self) -> ObjectState {
        self.inner.borrow().0
    }

    pub fn borrow(&self) -> Ref<'_, T> {
        if self.inner.borrow().0 == ObjectState::Removed {
            panic!("cannot borrow a removed object")
        }
        Ref::map(self.inner.borrow(), |x| {
            x.1.as_any().downcast_ref().unwrap()
        })
    }

    pub fn borrow_mut(&self) -> RefMut<'_, T> {
        self.inner.borrow_mut().0 = ObjectState::Modified;
        RefMut::map(self.inner.borrow_mut(), |x| {
            x.1.as_any_mut().downcast_mut().unwrap()
        })
    }

    pub fn delete(self) {
        self.inner
            .try_borrow_mut()
            .expect("cannot delete a borrowed object")
            .0 = ObjectState::Removed;
    }
}

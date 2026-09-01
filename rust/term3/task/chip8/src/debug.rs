use crate::{Address, OpCode, Sprite, Word};

pub trait DebugTarget {
    fn registers(&self) -> &[Word] {
        &[]
    }
    fn index_register(&self) -> Address {
        Address::default()
    }
    fn memory(&self) -> &[Word] {
        &[]
    }
    fn call_stack(&self) -> &[Address] {
        &[]
    }
    fn program_counter(&self) -> Address {
        Address::default()
    }
    fn opcode(&self) -> OpCode {
        OpCode::from_bytes(0, 0)
    }
    fn current_sprite(&self) -> Sprite<'_> {
        Sprite::new(&[])
    }
}

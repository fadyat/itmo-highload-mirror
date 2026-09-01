use crate::{
    Error, Key, Offset, Result,
    data::{Address, Nibble, OpCode, RegisterIndex, Word},
    image::Image,
    platform::{Platform, Point, Sprite},
};

////////////////////////////////////////////////////////////////////////////////

pub const SCREEN_WIDTH: usize = 64;
pub const SCREEN_HEIGHT: usize = 32;

pub const FONT_ADDRESS: Address = Address::new(0x0);
pub const FONT_HEIGHT: Offset = 5;
pub const FONT_SPRITES: [u8; 16 * FONT_HEIGHT as usize] = [
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F
];

////////////////////////////////////////////////////////////////////////////////

#[derive(Clone)]
pub struct Interpreter<P: Platform> {
    v_registers: [Word; 16],
    i_register: Address,
    memory: [u8; Address::DOMAIN_SIZE],
    stack: [Address; 16],
    sp: u8,
    pc: Address,
    platform: P,
    key_wait_state: Option<Word>,
}

impl<P: Platform> Interpreter<P> {
    pub fn new(image: impl Image, platform: P) -> Self {
        let mut memory = [0u8; 4096];

        memory[FONT_ADDRESS.as_usize()..FONT_ADDRESS.as_usize() + FONT_SPRITES.len()]
            .copy_from_slice(&FONT_SPRITES);

        image.load_into_memory(&mut memory);

        Self {
            v_registers: [0; 16],
            i_register: Address::new(0),
            memory,
            stack: [Address::new(0); 16],
            sp: 0,
            pc: image.entry_point(),
            platform,
            key_wait_state: None,
        }
    }

    pub fn platform(&self) -> &P {
        &self.platform
    }

    pub fn platform_mut(&mut self) -> &mut P {
        &mut self.platform
    }

    pub fn run_next_instruction(&mut self) -> Result<()> {
        let opcode = self.read_opcode();
        self.pc += 2 as Offset;

        let operation = Operation::try_from(opcode).map_err(|_| Error::UnknownOpCode(opcode))?;
        self.execute_operation(operation)
    }

    fn read_opcode(&self) -> OpCode {
        let high = self.memory[self.pc.as_usize()];
        let low = self.memory[self.pc.as_usize() + 1];
        OpCode::from_bytes(high, low)
    }

    fn vx(&self, r: RegisterIndex) -> Word {
        self.v_registers[r.as_usize()]
    }

    fn vx_mut(&mut self, r: RegisterIndex) -> &mut Word {
        &mut self.v_registers[r.as_usize()]
    }

    fn set_flag(&mut self, val: u8) {
        self.v_registers[0xF] = val;
    }

    fn mem_at(&mut self, offset: usize) -> &mut u8 {
        &mut self.memory[self.i_register.as_usize() + offset]
    }

    fn execute_operation(&mut self, operation: Operation) -> Result<()> {
        match operation {
            Operation::ClearScreen => {
                self.platform.clear_screen();
            }
            Operation::Return => {
                if self.sp == 0 {
                    return Err(Error::StackUnderflow);
                }
                self.sp -= 1;
                self.pc = self.stack[self.sp as usize];
            }
            Operation::Jump(address) => {
                self.pc = address;
            }
            Operation::Call(address) => {
                if self.sp >= 16 {
                    return Err(Error::StackOverflow);
                }
                self.stack[self.sp as usize] = self.pc;
                self.sp += 1;
                self.pc = address;
            }
            Operation::SkipIfEqual(reg, value) => {
                if self.vx(reg) == value {
                    self.pc += 2 as Offset;
                }
            }
            Operation::SkipIfNotEqual(reg, value) => {
                if self.vx(reg) != value {
                    self.pc += 2 as Offset;
                }
            }
            Operation::SkipIfRegistersEqual(reg1, reg2) => {
                if self.vx(reg1) == self.vx(reg2) {
                    self.pc += 2 as Offset;
                }
            }
            Operation::SetRegister(reg, value) => {
                *self.vx_mut(reg) = value;
            }
            Operation::AddValue(reg, value) => {
                let current = self.vx(reg);
                let result = current.wrapping_add(value);
                *self.vx_mut(reg) = result as Word;
            }
            Operation::SetToRegister(reg1, reg2) => {
                *self.vx_mut(reg1) = self.vx(reg2);
            }
            Operation::Or(reg1, reg2) => {
                let v1 = self.vx(reg1);
                let v2 = self.vx(reg2);
                *self.vx_mut(reg1) = v1 | v2;
                self.set_flag(0);
            }
            Operation::And(reg1, reg2) => {
                let v1 = self.vx(reg1);
                let v2 = self.vx(reg2);
                *self.vx_mut(reg1) = v1 & v2;
                self.set_flag(0);
            }
            Operation::Xor(reg1, reg2) => {
                let v1 = self.vx(reg1);
                let v2 = self.vx(reg2);
                *self.vx_mut(reg1) = v1 ^ v2;
                self.set_flag(0);
            }
            Operation::AddRegister(reg1, reg2) => {
                let old_vx = self.vx(reg1);
                let old_vy = self.vx(reg2);

                let sum = old_vx as u16 + old_vy as u16;
                let result = (sum & 0xFF) as u8;
                let carry = if sum > 0xFF { 1 } else { 0 };

                *self.vx_mut(reg1) = result;
                self.set_flag(carry);
            }
            Operation::SubRegister(reg1, reg2) => {
                let old_vx = self.vx(reg1);
                let old_vy = self.vx(reg2);

                let result = old_vx.wrapping_sub(old_vy);
                let borrow = if old_vx >= old_vy { 1 } else { 0 };

                *self.vx_mut(reg1) = result;
                self.set_flag(borrow);
            }
            Operation::ShiftRight(reg1, reg2) => {
                let vy = self.vx(reg2);
                let lsb = vy & 0x1;
                *self.vx_mut(reg1) = vy >> 1;
                self.set_flag(lsb);
            }
            Operation::SubRegisterReversed(reg1, reg2) => {
                let old_vx = self.vx(reg1);
                let old_vy = self.vx(reg2);

                let result = old_vy.wrapping_sub(old_vx);
                let borrow = if old_vy >= old_vx { 1 } else { 0 };

                *self.vx_mut(reg1) = result;
                self.set_flag(borrow);
            }
            Operation::ShiftLeft(reg1, reg2) => {
                let vy = self.vx(reg2);
                let msb = (vy >> 7) & 0x1;
                *self.vx_mut(reg1) = vy << 1;
                self.set_flag(msb);
            }
            Operation::SkipIfRegistersNotEqual(reg1, reg2) => {
                if self.vx(reg1) != self.vx(reg2) {
                    self.pc += 2 as Offset;
                }
            }
            Operation::SetIndexRegister(address) => {
                self.i_register = address;
            }
            Operation::JumpV0(address) => {
                let v0 = self.v_registers[0] as u16;
                self.pc = address + (v0 as Offset);
            }
            Operation::Draw(x_reg, y_reg, height) => {
                let x = self.vx(x_reg) % SCREEN_WIDTH as u8;
                let y = self.vx(y_reg) % SCREEN_HEIGHT as u8;
                let height = height.as_u8() as usize;

                let sprite_data =
                    &self.memory[self.i_register.as_usize()..self.i_register.as_usize() + height];
                let sprite = Sprite::new(sprite_data);

                let collision = self.platform.draw_sprite(Point { x, y }, sprite);

                self.set_flag(if collision { 1 } else { 0 });
            }
            Operation::SkipIfKeyDown(reg) => {
                let key_value = self.vx(reg) & 0xF;
                let key = Key::try_from(key_value).map_err(|()| Error::InvalidKey(key_value))?;
                if self.platform.is_key_down(key) {
                    self.pc += 2 as Offset;
                }
            }
            Operation::SkipIfKeyUp(reg) => {
                let key_value = self.vx(reg) & 0xF;
                let key = Key::try_from(key_value).map_err(|()| Error::InvalidKey(key_value))?;
                if !self.platform.is_key_down(key) {
                    self.pc += 2 as Offset;
                }
            }
            Operation::GetDelayTimer(reg) => {
                let delay = self.platform.get_delay_timer();
                *self.vx_mut(reg) = delay;
            }
            Operation::WaitForKey(reg) => match self.key_wait_state {
                None => {
                    for i in 0..16 {
                        if let Ok(key) = Key::try_from(i as u8)
                            && self.platform.is_key_down(key)
                        {
                            *self.vx_mut(reg) = i as Word;
                            self.key_wait_state = Some(i as Word);
                            self.pc += (-2) as Offset;
                            return Ok(());
                        }
                    }
                    self.pc += (-2) as Offset;
                }
                Some(key_code) => {
                    if let Ok(key) = Key::try_from(key_code) {
                        if self.platform.is_key_down(key) {
                            self.pc += (-2) as Offset;
                        } else {
                            self.key_wait_state = None;
                        }
                    } else {
                        self.key_wait_state = None;
                    }
                }
            },
            Operation::SetDelayTimer(reg) => {
                let value = self.vx(reg);
                self.platform.set_delay_timer(value);
            }
            Operation::IncrementIndexRegister(reg) => {
                let value = self.vx(reg) as u16;
                self.i_register += value as Offset;
            }
            Operation::ToDecimal(reg) => {
                let value = self.vx(reg);
                let hundreds = value / 100;
                let tens = (value % 100) / 10;
                let ones = value % 10;

                *self.mem_at(0) = hundreds;
                *self.mem_at(1) = tens;
                *self.mem_at(2) = ones;
            }
            Operation::WriteMemory(n) => {
                let n = n.as_usize();
                for i in 0..=n {
                    *self.mem_at(i) = self.v_registers[i];
                }
                self.i_register += ((n as u16) + 1) as Offset;
            }
            Operation::ReadMemory(n) => {
                let n = n.as_usize();
                for i in 0..=n {
                    self.v_registers[i] = *self.mem_at(i) as Word;
                }
                self.i_register += ((n as u16) + 1) as Offset;
            }
            _ => {}
        }
        Ok(())
    }
}

////////////////////////////////////////////////////////////////////////////////

#[derive(Debug, Clone, Copy)]
pub enum Operation {
    ClearScreen,
    Return,
    Jump(Address),
    Call(Address),
    SkipIfEqual(RegisterIndex, Word),
    SkipIfNotEqual(RegisterIndex, Word),
    SkipIfRegistersEqual(RegisterIndex, RegisterIndex),
    SetRegister(RegisterIndex, Word),
    AddValue(RegisterIndex, Word),
    SetToRegister(RegisterIndex, RegisterIndex),
    Or(RegisterIndex, RegisterIndex),
    And(RegisterIndex, RegisterIndex),
    Xor(RegisterIndex, RegisterIndex),
    AddRegister(RegisterIndex, RegisterIndex),
    SubRegister(RegisterIndex, RegisterIndex),
    ShiftRight(RegisterIndex, RegisterIndex),
    SubRegisterReversed(RegisterIndex, RegisterIndex),
    ShiftLeft(RegisterIndex, RegisterIndex),
    SkipIfRegistersNotEqual(RegisterIndex, RegisterIndex),
    SetIndexRegister(Address),
    JumpV0(Address),
    SetToRandom(RegisterIndex, Word),
    Draw(RegisterIndex, RegisterIndex, Nibble),
    SkipIfKeyDown(RegisterIndex),
    SkipIfKeyUp(RegisterIndex),
    GetDelayTimer(RegisterIndex),
    WaitForKey(RegisterIndex),
    SetDelayTimer(RegisterIndex),
    SetSoundTimer(RegisterIndex),
    IncrementIndexRegister(RegisterIndex),
    SetIndexRegisterToSprite(Nibble),
    ToDecimal(RegisterIndex),
    WriteMemory(Nibble),
    ReadMemory(Nibble),
}

impl TryFrom<OpCode> for Operation {
    type Error = ();

    fn try_from(code: OpCode) -> core::result::Result<Self, ()> {
        let op = match code.as_u16() {
            0x00E0 => Operation::ClearScreen,
            0x00EE => Operation::Return,
            op if (op & 0xF000) == 0x1000 => Operation::Jump(code.extract_address()),
            op if (op & 0xF000) == 0x2000 => Operation::Call(code.extract_address()),
            op if (op & 0xF000) == 0x3000 => {
                Operation::SkipIfEqual(code.extract_nibble(1), code.extract_word(1))
            }
            op if (op & 0xF000) == 0x4000 => {
                Operation::SkipIfNotEqual(code.extract_nibble(1), code.extract_word(1))
            }
            op if (op & 0xF000) == 0x5000 => {
                Operation::SkipIfRegistersEqual(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF000) == 0x6000 => {
                Operation::SetRegister(code.extract_nibble(1), code.extract_word(1))
            }
            op if (op & 0xF000) == 0x7000 => {
                Operation::AddValue(code.extract_nibble(1), code.extract_word(1))
            }
            op if (op & 0xF00F) == 0x8000 => {
                Operation::SetToRegister(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8001 => {
                Operation::Or(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8002 => {
                Operation::And(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8003 => {
                Operation::Xor(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8004 => {
                Operation::AddRegister(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8005 => {
                Operation::SubRegister(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8006 => {
                Operation::ShiftRight(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x8007 => {
                Operation::SubRegisterReversed(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF00F) == 0x800E => {
                Operation::ShiftLeft(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF000) == 0x9000 => {
                Operation::SkipIfRegistersNotEqual(code.extract_nibble(1), code.extract_nibble(2))
            }
            op if (op & 0xF000) == 0xA000 => Operation::SetIndexRegister(code.extract_address()),
            op if (op & 0xF000) == 0xB000 => Operation::JumpV0(Address::new(op & 0x0FFF)),
            op if (op & 0xF000) == 0xD000 => Operation::Draw(
                code.extract_nibble(1),
                code.extract_nibble(2),
                code.extract_nibble(3),
            ),
            op if (op & 0xF0FF) == 0xE09E => Operation::SkipIfKeyDown(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xE0A1 => Operation::SkipIfKeyUp(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF007 => Operation::GetDelayTimer(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF00A => Operation::WaitForKey(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF015 => Operation::SetDelayTimer(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF01E => {
                Operation::IncrementIndexRegister(code.extract_nibble(1))
            }
            op if (op & 0xF0FF) == 0xF033 => Operation::ToDecimal(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF055 => Operation::WriteMemory(code.extract_nibble(1)),
            op if (op & 0xF0FF) == 0xF065 => Operation::ReadMemory(code.extract_nibble(1)),
            _ => return Err(()),
        };
        Ok(op)
    }
}

////////////////////////////////////////////////////////////////////////////////

impl<P: Platform> crate::DebugTarget for Interpreter<P> {
    // TODO: your code here.
}

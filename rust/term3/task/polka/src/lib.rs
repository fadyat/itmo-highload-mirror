#![forbid(unsafe_code)]

use std::{collections::HashMap, fmt::Display};

#[derive(Clone, Debug, PartialEq)]
pub enum Value {
    Number(f64),
    Symbol(String),
}

impl Display for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Number(num) => write!(f, "{}", num),
            Self::Symbol(sym) => write!(f, "'{}", sym),
        }
    }
}

pub struct Interpreter {
    kv: HashMap<String, Value>,
    stack: Vec<Value>,
}

impl Default for Interpreter {
    fn default() -> Self {
        Self::new()
    }
}

impl Interpreter {
    pub fn new() -> Self {
        Self {
            kv: HashMap::new(),
            stack: Vec::new(),
        }
    }

    pub fn eval(&mut self, expr: &str) {
        for token in expr.split_whitespace() {
            match token {
                "+" | "-" | "*" | "/" => {
                    let (a, b) = {
                        let a = self.stack.pop().expect("Stack underflow");
                        let b = self.stack.pop().expect("Stack underflow");

                        match (a, b) {
                            (Value::Number(a), Value::Number(b)) => (a, b),
                            _ => panic!("Expected two numbers"),
                        }
                    };

                    let result = match token {
                        "+" => a + b,
                        "-" => a - b,
                        "*" => a * b,
                        "/" => a / b,
                        _ => unreachable!(),
                    };

                    self.stack.push(Value::Number(result));
                }
                "set" => {
                    let name = self.stack.pop().expect("Stack underflow");
                    let value = self.stack.pop().expect("Stack underflow");

                    self.kv.insert(
                        match name {
                            Value::Symbol(s) => s,
                            _ => panic!("Expected symbol, got: {}", name),
                        },
                        value,
                    );
                }
                token if token.starts_with('$') => {
                    self.kv.get(&token[1..]).map_or_else(
                        || panic!("Undefined variable: {}", &token[1..]),
                        |val| self.stack.push(val.clone()),
                    );
                }
                token if token.starts_with('\'') => {
                    self.stack.push(Value::Symbol(token[1..].to_string()));
                }
                token => {
                    token.parse::<f64>().map_or_else(
                        |_| panic!("Unexpected token: {}", token),
                        |num| self.stack.push(Value::Number(num)),
                    );
                }
            }
        }
    }

    pub fn stack(&self) -> &[Value] {
        &self.stack
    }
}

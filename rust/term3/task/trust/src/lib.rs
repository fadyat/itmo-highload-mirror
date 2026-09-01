#![forbid(unsafe_code)]

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum RoundOutcome {
    BothCooperated,
    LeftCheated,
    RightCheated,
    BothCheated,
}

#[derive(Clone, Copy, PartialEq, Eq)]
pub enum Move {
    Cooperate,
    Cheat,
}

pub trait Agent {
    fn play(&mut self) -> Move;
    fn learn(&mut self, opponent_move: Move);
}

pub struct Game {
    left: Box<dyn Agent>,
    right: Box<dyn Agent>,
    left_score: i32,
    right_score: i32,
}

impl Game {
    pub fn new(left: Box<dyn Agent>, right: Box<dyn Agent>) -> Self {
        Self {
            left,
            right,
            left_score: 0,
            right_score: 0,
        }
    }

    pub fn left_score(&self) -> i32 {
        self.left_score
    }

    pub fn right_score(&self) -> i32 {
        self.right_score
    }

    pub fn play_round(&mut self) -> RoundOutcome {
        let left_move = self.left.play();
        let right_move = self.right.play();

        self.left.learn(right_move);
        self.right.learn(left_move);

        match (left_move, right_move) {
            (Move::Cooperate, Move::Cooperate) => {
                self.left_score += 2;
                self.right_score += 2;
                RoundOutcome::BothCooperated
            }
            (Move::Cooperate, Move::Cheat) => {
                self.left_score -= 1;
                self.right_score += 3;
                RoundOutcome::RightCheated
            }
            (Move::Cheat, Move::Cooperate) => {
                self.left_score += 3;
                self.right_score -= 1;
                RoundOutcome::LeftCheated
            }
            (Move::Cheat, Move::Cheat) => RoundOutcome::BothCheated,
        }
    }
}

#[derive(Default)]
pub struct CheatingAgent {}

impl CheatingAgent {
    pub fn new() -> Self {
        Default::default()
    }
}

impl Agent for CheatingAgent {
    fn play(&mut self) -> Move {
        Move::Cheat
    }

    fn learn(&mut self, _opponent_move: Move) {}
}

#[derive(Default)]
pub struct CooperatingAgent {}

impl CooperatingAgent {
    pub fn new() -> Self {
        Default::default()
    }
}

impl Agent for CooperatingAgent {
    fn play(&mut self) -> Move {
        Move::Cooperate
    }

    fn learn(&mut self, _opponent_move: Move) {}
}

#[derive(Default)]
pub struct GrudgerAgent {
    opponent_ever_cheated: bool,
}

impl GrudgerAgent {
    pub fn new() -> Self {
        Default::default()
    }
}

impl Agent for GrudgerAgent {
    fn play(&mut self) -> Move {
        if self.opponent_ever_cheated {
            Move::Cheat
        } else {
            Move::Cooperate
        }
    }

    fn learn(&mut self, opponent_move: Move) {
        if let Move::Cheat = opponent_move {
            self.opponent_ever_cheated = true;
        }
    }
}

#[derive(Default)]
pub struct CopycatAgent {
    previous_opponent_move: Option<Move>,
}

impl CopycatAgent {
    pub fn new() -> Self {
        Default::default()
    }
}

impl Agent for CopycatAgent {
    fn play(&mut self) -> Move {
        self.previous_opponent_move.unwrap_or(Move::Cooperate)
    }

    fn learn(&mut self, opponent_move: Move) {
        self.previous_opponent_move = Some(opponent_move)
    }
}

#[derive(Default)]
pub struct DetectiveAgent {
    move_number: i32,
    opponent_ever_cheated: bool,
    previous_opponent_move: Option<Move>,
}

impl DetectiveAgent {
    pub fn new() -> Self {
        Default::default()
    }
}

impl Agent for DetectiveAgent {
    fn play(&mut self) -> Move {
        let player_move = match self.move_number {
            0 => Move::Cooperate,
            1 => Move::Cheat,
            2 => Move::Cooperate,
            3 => Move::Cooperate,
            _ => {
                if self.opponent_ever_cheated {
                    self.previous_opponent_move.unwrap()
                } else {
                    Move::Cheat
                }
            }
        };
        self.move_number += 1;
        player_move
    }

    fn learn(&mut self, opponent_move: Move) {
        self.previous_opponent_move = Some(opponent_move);
        if opponent_move == Move::Cheat {
            self.opponent_ever_cheated = true;
        }
    }
}

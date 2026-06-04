use std::collections::LinkedList;

use paperio_proto::{Cell, Direction, World};

impl Default for Strategy {
    fn default() -> Self {
        Self::new()
    }
}

struct Rectangle {
    llc: Cell,
    urc: Cell,
}

impl Rectangle {
    fn new(llc: Cell, urc: Cell) -> Self {
        assert!(llc.0 < urc.0 && llc.1 < urc.1);
        Self { llc, urc }
    }

    fn perimeter(&self) -> i32 {
        let w = self.urc.0 - self.llc.0;
        let h = self.urc.1 - self.llc.1;
        2 * (w + h)
    }

    fn iter_cells(&self) -> impl Iterator<Item = Cell> {
        (self.llc.0..=self.urc.0)
            .flat_map(move |x| (self.llc.1..=self.urc.1).map(move |y| Cell(x, y)))
    }

    #[rustfmt::skip]
    fn iter_perimeter(&self) -> impl Iterator<Item = Cell> {
        let bottom = (self.llc.0..=self.urc.0).map(move |x| Cell(x, self.llc.1));
        let right = (self.llc.1 + 1..=self.urc.1).map(move |y| Cell(self.urc.0, y));
        let top = (self.llc.0..self.urc.0).rev().map(move |x| Cell(x, self.urc.1));
        let left = (self.llc.1 + 1..self.urc.1).rev().map(move |y| Cell(self.llc.0, y));
        bottom.chain(right).chain(top).chain(left)
    }
}

pub struct Strategy {
    best_rectangle: Option<Rectangle>,
    route: LinkedList<Cell>,
}

impl Strategy {
    pub fn new() -> Self {
        Self {
            best_rectangle: None,
            route: LinkedList::new(),
        }
    }

    pub fn on_tick(&mut self, world: World) -> Direction {
        let player = world.me();

        if player.territory.contains(&player.position) {
            self.best_rectangle = self.find_best_rectangle(&world).into();
            self.route = self.calculate_route(player.position, player.direction);
        }

        self.route
            .pop_front()
            .map(|next_cell| player.position.direction_to(next_cell))
            .expect("route must contain at least one cell")
    }

    fn calculate_route(
        &self,
        player_position: Cell,
        previous_direction: Option<Direction>,
    ) -> LinkedList<Cell> {
        let mut perimeter = self
            .best_rectangle
            .as_ref()
            .expect("best rectangle must be set")
            .iter_perimeter()
            .collect::<Vec<_>>();

        let start_index = perimeter
            .iter()
            .position(|&cell| cell == player_position)
            .expect("player must be on the perimeter of the rectangle");

        perimeter.rotate_left(start_index + 1);

        let previous_direction = previous_direction.unwrap_or(Direction::Left);
        let next_direction = player_position.direction_to(perimeter[0]);

        if next_direction == previous_direction.opposite() {
            let final_position = perimeter.pop();
            perimeter.reverse();
            perimeter.push(final_position.unwrap());
        }

        perimeter.into_iter().collect()
    }

    // find_best_rectangle looking for best rectangle (with max score),
    // where player located in one of corners of this rectangle.
    fn find_best_rectangle(&self, world: &World) -> Rectangle {
        let player_corner = world.me().position;

        world
            .iter_cells()
            .filter(|cell| cell.0 != player_corner.0 && cell.1 != player_corner.1)
            .map(|cell| {
                Rectangle::new(
                    Cell(player_corner.0.min(cell.0), player_corner.1.min(cell.1)),
                    Cell(player_corner.0.max(cell.0), player_corner.1.max(cell.1)),
                )
            })
            .max_by_key(|rectangle| self.calculate_rectangle_score(rectangle, world))
            .expect("there is always at least one rectangle")
    }

    fn calculate_rectangle_score(&self, rectangle: &Rectangle, world: &World) -> i32 {
        let cells_score = self.calculate_cells_score(rectangle, world);
        let danger = self.calculate_danger_score(rectangle, world);

        cells_score - danger * danger
    }

    // calculate_cells_score returns summed score for all cells in rectangle,
    // where each cell have there own score depending on its state.
    fn calculate_cells_score(&self, rectangle: &Rectangle, world: &World) -> i32 {
        rectangle
            .iter_cells()
            .map(|cell| self.cell_score(cell, world))
            .sum()
    }

    fn cell_score(&self, cell: Cell, world: &World) -> i32 {
        if world.me().territory.contains(&cell) {
            return 0;
        }

        for (_, player) in world.iter_enemies() {
            if player.territory.contains(&cell) {
                return 5;
            } else if player.lines.contains(&cell) {
                return 3;
            }
        }

        1
    }

    // calculate_danger_score returns the perimeter length minus the minimum distance from any enemy
    // to any cell on the perimeter of the rectangle.
    fn calculate_danger_score(&self, rectangle: &Rectangle, world: &World) -> i32 {
        let min_enemy_distance = rectangle
            .iter_perimeter()
            .flat_map(|cell| {
                world
                    .iter_enemies()
                    .map(move |(_, player)| cell.distance_to(player.position))
            })
            .min()
            .unwrap_or(rectangle.perimeter());

        rectangle.perimeter() - min_enemy_distance
    }
}

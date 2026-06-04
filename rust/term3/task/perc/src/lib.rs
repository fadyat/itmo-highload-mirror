use rand::Rng;

/// Represents a grid of boolean values.
pub struct BoolGrid {
    width: usize,
    height: usize,
    cells: Vec<bool>,
}

impl BoolGrid {
    /// Creates a new grid with all values initialized as `false`.
    ///
    /// # Arguments
    ///
    /// * `width` - grid width.
    /// * `height` - grid height.
    pub fn new(width: usize, height: usize) -> Self {
        Self {
            width,
            height,
            cells: vec![false; width * height],
        }
    }

    /// Creates a new grid with every value initialized randomly.
    ///
    /// # Arguments
    ///
    /// * `width` - grid width.
    /// * `height` - grid height.
    /// * `vacancy` - probability of any given value being equal
    ///   to `false`.
    pub fn random(width: usize, height: usize, vacancy: f64) -> Self {
        let mut rng = rand::rng();
        let cells = (0..width * height)
            .map(|_| rng.random::<f64>() >= vacancy)
            .collect();

        Self {
            width,
            height,
            cells,
        }
    }

    /// Returns grid width.
    pub fn width(&self) -> usize {
        self.width
    }

    /// Returns grid height.
    pub fn height(&self) -> usize {
        self.height
    }

    /// Returns the current value of a given cell.
    /// The caller must ensure that `x` and `y` are valid.
    ///
    /// # Arguments
    ///
    /// * `x` - must be >= 0 and < grid width.
    /// * `y` - must be >= 0 and < grid height.
    ///
    /// # Panics
    ///
    /// If `x` or `y` is out of bounds, this method may panic
    /// (or return incorrect result).
    pub fn get(&self, x: usize, y: usize) -> bool {
        self.cells[y * self.width + x]
    }

    /// Sets a new value to a given cell.
    /// The caller must ensure that `x` and `y` are valid.
    ///
    /// # Arguments
    ///
    /// * `x` - must be >= 0 and < grid width.
    /// * `y` - must be >= 0 and < grid height.
    ///
    /// # Panics
    ///
    /// If `x` or `y` is out of bounds, this method may panic
    /// (or set value to some other unspecified cell).
    pub fn set(&mut self, x: usize, y: usize, value: bool) {
        self.cells[y * self.width + x] = value;
    }

    fn bounds_check(&self, x: isize, y: isize) -> bool {
        x >= 0 && x < self.width as isize && y >= 0 && y < self.height as isize
    }

    fn dfs(&self, x: usize, y: usize, visited: &mut Vec<bool>) -> bool {
        let idx = y * self.width + x;
        if visited[idx] || self.get(x, y) {
            return false;
        }

        visited[idx] = true;
        if y == self.height - 1 {
            return true;
        }

        const DIRECTIONS: [(isize, isize); 4] = [(-1, 0), (1, 0), (0, -1), (0, 1)];

        DIRECTIONS.iter().any(|(dx, dy)| {
            let new_x = x as isize + dx;
            let new_y = y as isize + dy;
            self.bounds_check(new_x, new_y) && self.dfs(new_x as usize, new_y as usize, visited)
        })
    }
}

////////////////////////////////////////////////////////////////////////////////

/// Returns `true` if the given grid percolates. That is, if there is a path
/// from any cell with `y` == 0 to any cell with `y` == `height` - 1.
/// If the grid is empty (`width` == 0 or `height` == 0), it percolates.
pub fn percolates(grid: &BoolGrid) -> bool {
    if grid.width() == 0 || grid.height() == 0 {
        return true;
    }

    let mut visited = vec![false; grid.width() * grid.height()];
    (0..grid.width()).any(|x| !grid.get(x, 0) && grid.dfs(x, 0, &mut visited))
}

////////////////////////////////////////////////////////////////////////////////

const N_TRIALS: u64 = 10000;

/// Returns an estimate of the probability that a random grid with given
/// `width, `height` and `vacancy` probability percolates.
/// To compute an estimate, it runs `N_TRIALS` of random experiments,
/// in each creating a random grid and checking if it percolates.
pub fn evaluate_probability(width: usize, height: usize, vacancy: f64) -> f64 {
    let mut perc_count = 0;
    for _ in 0..N_TRIALS {
        let grid = BoolGrid::random(width, height, vacancy);
        if percolates(&grid) {
            perc_count += 1;
        }
    }
    perc_count as f64 / N_TRIALS as f64
}

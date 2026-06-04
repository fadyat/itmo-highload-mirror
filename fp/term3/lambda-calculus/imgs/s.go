package main

import "fmt"

func S[Z, T1, T3 any](
	x func(Z, T1) T3,
	y func(Z) T1,
	z Z,
) T3 {
	return x(z, y(z))
}

func main() {
	x := func(a int, b float64) string {
		return fmt.Sprintf("%d -> %.1f", a, b)
	}

	y := func(a int) float64 {
		return float64(a) * 2.5
	}

	z := 4
	result := S(x, y, z)
	fmt.Println(result)
}

package main

import "fmt"

type Pair[T any, U any] func(func(T, U) any) any

func MakePair[T any, U any](a T, b U) Pair[T, U] {
	return func(f func(T, U) any) any {
		return f(a, b)
	}
}

func First[T any, U any](p Pair[T, U]) T {
	return p(func(a T, _ U) any { return a }).(T)
}

func Second[T any, U any](p Pair[T, U]) U {
	return p(func(_ T, b U) any { return b }).(U)
}

type Cat = Pair[string, int]

func MakeCat(name string, purr int) Cat {
	return MakePair(name, purr)
}

func CatName(c Cat) string {
	return First(c)
}

func CatPurr(c Cat) int {
	return Second(c)
}

func Pet(c Cat) Cat {
	return MakeCat(CatName(c), CatPurr(c)+1)
}

func main() {
	kitty := MakeCat("Mittens", 5)
	fmt.Println("Name:", CatName(kitty))
	fmt.Println("Purr:", CatPurr(kitty))

	kitty = Pet(kitty)
	fmt.Println("After petting, Purr:", CatPurr(kitty))
}

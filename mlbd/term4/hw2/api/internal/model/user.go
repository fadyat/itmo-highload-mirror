package model

import "time"

type User struct {
	UserID    string    `db:"user_id"    json:"user_id"`
	Name      string    `db:"name"       json:"name"`
	Email     string    `db:"email"      json:"email"`
	Country   string    `db:"country"    json:"country"`
	CreatedAt time.Time `db:"created_at" json:"created_at"`
}

func NewUser(id, name, email, country string) *User {
	return &User{
		UserID:    id,
		Name:      name,
		Email:     email,
		Country:   country,
		CreatedAt: time.Now().UTC(),
	}
}

package model

import "time"

type User struct {
	UserID    string    `json:"user_id"    bson:"user_id"`
	Name      string    `json:"name"       bson:"name"`
	Email     string    `json:"email"      bson:"email"`
	Country   string    `json:"country"    bson:"country"`
	CreatedAt time.Time `json:"created_at" bson:"created_at"`
}

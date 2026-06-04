package dto

import (
	"time"

	"github.com/samber/lo"

	"github.com/fadyat/mlbd/hw2/api/internal/model"
)

type CreateUserRequest struct {
	Name    string `json:"name"    validate:"required,min=1,max=255"`
	Email   string `json:"email"   validate:"required,email"`
	Country string `json:"country" validate:"required,iso3166_1_alpha2"`
}

type ListUsersQuery struct {
	Country string `form:"country" validate:"required,iso3166_1_alpha2"`
	Limit   int    `form:"limit"   validate:"omitempty,min=1,max=1000"`
}

type UserResponse struct {
	UserID    string    `json:"user_id"`
	Name      string    `json:"name"`
	Email     string    `json:"email"`
	Country   string    `json:"country"`
	CreatedAt time.Time `json:"created_at"`
}

func FromUser(u *model.User) *UserResponse {
	if u == nil {
		return nil
	}
	return &UserResponse{
		UserID:    u.UserID,
		Name:      u.Name,
		Email:     u.Email,
		Country:   u.Country,
		CreatedAt: u.CreatedAt,
	}
}

func FromUsers(users []model.User) []UserResponse {
	return lo.Map(users, func(u model.User, _ int) UserResponse {
		return *FromUser(&u)
	})
}

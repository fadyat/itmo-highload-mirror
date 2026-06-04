package store

import (
	"context"
	"errors"
	"fmt"

	"github.com/jackc/pgx/v5/pgconn"
	"github.com/jmoiron/sqlx"

	"github.com/fadyat/mlbd/hw2/api/internal/model"
)

const (
	pgUniqueViolation = "23505"
	defaultListLimit  = 100
	maxListLimit      = 1000
)

type UserStore struct {
	db *sqlx.DB
}

func NewUserStore(dsn string) (*UserStore, error) {
	db, err := sqlx.Connect("pgx", dsn)
	if err != nil {
		return nil, fmt.Errorf("sqlx.Connect: %w", err)
	}

	return &UserStore{db: db}, nil
}

func (s *UserStore) Close() error { return s.db.Close() }

func (s *UserStore) Insert(ctx context.Context, u *model.User) error {
	const q = `insert into users (user_id, name, email, country, created_at)
	           values ($1, $2, $3, $4, $5)`

	_, err := s.db.ExecContext(ctx, q, u.UserID, u.Name, u.Email, u.Country, u.CreatedAt)
	if pgErr, ok := errors.AsType[*pgconn.PgError](err); ok && pgErr.Code == pgUniqueViolation {
		return errors.New("email already exists")
	}
	if err != nil {
		return fmt.Errorf("sqlx.DB.ExecContext: %w", err)
	}

	return nil
}

func (s *UserStore) GetByID(ctx context.Context, id string) (*model.User, error) {
	const q = `select user_id, name, email, country, created_at from users where user_id = $1`

	var u model.User
	if err := s.db.GetContext(ctx, &u, q, id); err != nil {
		return nil, fmt.Errorf("sqlx.DB.GetContext: %w", err)
	}

	return &u, nil
}

func (s *UserStore) ListByCountry(ctx context.Context, country string, limit int) ([]model.User, error) {
	if limit <= 0 || limit > maxListLimit {
		limit = defaultListLimit
	}

	const q = `select user_id, name, email, country, created_at
	           from users where country = $1 order by created_at desc limit $2`

	users := make([]model.User, 0, limit)
	if err := s.db.SelectContext(ctx, &users, q, country, limit); err != nil {
		return nil, fmt.Errorf("sqlx.DB.SelectContext: %w", err)
	}

	return users, nil
}

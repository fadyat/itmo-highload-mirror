package httpapi

import (
	"fmt"
	"net/http"

	"github.com/google/uuid"
	"go.uber.org/zap"

	"github.com/fadyat/mlbd/hw2/api/internal/dto"
	"github.com/fadyat/mlbd/hw2/api/internal/dumper"
	"github.com/fadyat/mlbd/hw2/api/internal/model"
	"github.com/fadyat/mlbd/hw2/api/internal/store"
)

type Handler struct {
	store  *store.UserStore
	dumper *dumper.Dumper
	log    *zap.Logger
}

func NewHandler(s *store.UserStore, d *dumper.Dumper, log *zap.Logger) *Handler {
	return &Handler{store: s, dumper: d, log: log}
}

func (h *Handler) Routes() *http.ServeMux {
	mux := http.NewServeMux()
	mux.HandleFunc("GET /healthz", func(w http.ResponseWriter, _ *http.Request) {
		w.WriteHeader(http.StatusOK)
	})
	mux.Handle("POST /users", wrap(h.createUser, http.StatusCreated, h.log))
	mux.Handle("GET /users/{id}", wrap(h.getUser, http.StatusOK, h.log))
	mux.Handle("GET /users", wrap(h.listUsers, http.StatusOK, h.log))
	return mux
}

func (h *Handler) createUser(r *http.Request) (*dto.UserResponse, error) {
	req, err := bindJSON[dto.CreateUserRequest](r)
	if err != nil {
		return nil, err
	}

	u := model.NewUser(uuid.Must(uuid.NewV7()).String(), req.Name, req.Email, req.Country)
	if err := h.store.Insert(r.Context(), u); err != nil {
		return nil, conflict(err.Error())
	}

	h.dumper.Add(*u)
	return dto.FromUser(u), nil
}

func (h *Handler) getUser(r *http.Request) (*dto.UserResponse, error) {
	u, err := h.store.GetByID(r.Context(), r.PathValue("id"))
	if err != nil {
		return nil, notFound("not found")
	}
	return dto.FromUser(u), nil
}

func (h *Handler) listUsers(r *http.Request) ([]dto.UserResponse, error) {
	q, err := bindQuery[dto.ListUsersQuery](r)
	if err != nil {
		return nil, err
	}

	users, err := h.store.ListByCountry(r.Context(), q.Country, q.Limit)
	if err != nil {
		return nil, fmt.Errorf("store.UserStore.ListByCountry: %w", err)
	}

	return dto.FromUsers(users), nil
}

package httpapi

import (
	"encoding/json"
	"errors"
	"net/http"

	"go.uber.org/zap"
)

type APIError struct {
	Status int
	Msg    string
}

func (e *APIError) Error() string { return e.Msg }

func badRequest(msg string) error { return &APIError{Status: http.StatusBadRequest, Msg: msg} }
func notFound(msg string) error   { return &APIError{Status: http.StatusNotFound, Msg: msg} }
func conflict(msg string) error   { return &APIError{Status: http.StatusConflict, Msg: msg} }

func wrap[T any](h func(*http.Request) (T, error), okStatus int, log *zap.Logger) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		body, err := h(r)
		if err != nil {
			if ae, ok := errors.AsType[*APIError](err); ok {
				writeJSON(w, ae.Status, errBody{Error: ae.Msg})
				return
			}
			log.Error("handler", zap.String("path", r.URL.Path), zap.Error(err))
			writeJSON(w, http.StatusInternalServerError, errBody{Error: "internal"})
			return
		}
		writeJSON(w, okStatus, body)
	}
}

type errBody struct {
	Error string `json:"error"`
}

func writeJSON(w http.ResponseWriter, code int, v any) {
	w.Header().Set("Content-Type", "application/json")
	w.WriteHeader(code)
	//nolint:errchkjson // generic response body, headers already sent
	_ = json.NewEncoder(w).Encode(v)
}

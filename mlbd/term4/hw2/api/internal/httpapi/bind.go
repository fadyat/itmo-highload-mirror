package httpapi

import (
	"encoding/json"
	"net/http"

	"github.com/go-playground/form/v4"
)

var formDecoder = func() *form.Decoder {
	d := form.NewDecoder()
	d.SetTagName("form")
	return d
}()

func bindJSON[T any](r *http.Request) (T, error) {
	var v T
	if err := json.NewDecoder(r.Body).Decode(&v); err != nil {
		return v, badRequest("invalid json")
	}
	if err := validateStruct(&v); err != nil {
		return v, badRequest(err.Error())
	}
	return v, nil
}

func bindQuery[T any](r *http.Request) (T, error) {
	var v T
	if err := formDecoder.Decode(&v, r.URL.Query()); err != nil {
		return v, badRequest("invalid query: " + err.Error())
	}
	if err := validateStruct(&v); err != nil {
		return v, badRequest(err.Error())
	}
	return v, nil
}

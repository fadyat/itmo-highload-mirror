package httpapi

import (
	"errors"
	"fmt"
	"strings"

	"github.com/go-playground/validator/v10"
)

var validate = validator.New(validator.WithRequiredStructEnabled())

func validateStruct(s any) error {
	err := validate.Struct(s)
	if err == nil {
		return nil
	}
	ve, ok := errors.AsType[validator.ValidationErrors](err)
	if !ok {
		return fmt.Errorf("validator.Validate.Struct: %w", err)
	}
	parts := make([]string, 0, len(ve))
	for _, fe := range ve {
		parts = append(parts, fe.Field()+": "+fe.Tag())
	}
	return errors.New(strings.Join(parts, "; "))
}

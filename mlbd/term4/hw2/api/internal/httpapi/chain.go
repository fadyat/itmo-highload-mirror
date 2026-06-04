package httpapi

import (
	"net/http"
	"runtime/debug"
	"time"

	"go.uber.org/zap"
)

type Middleware func(http.Handler) http.Handler

// Chain wraps h with the given middlewares; the leftmost is outermost
// (first to see the request).
func Chain(h http.Handler, mw ...Middleware) http.Handler {
	for i := len(mw) - 1; i >= 0; i-- {
		h = mw[i](h)
	}
	return h
}

func Recoverer(log *zap.Logger) Middleware {
	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			defer func() {
				if rec := recover(); rec != nil {
					log.Error("panic",
						zap.Any("recover", rec),
						zap.String("path", r.URL.Path),
						zap.ByteString("stack", debug.Stack()),
					)
					http.Error(w, `{"error":"internal"}`, http.StatusInternalServerError)
				}
			}()
			next.ServeHTTP(w, r)
		})
	}
}

type statusRecorder struct {
	http.ResponseWriter
	status int
}

func (s *statusRecorder) WriteHeader(code int) {
	s.status = code
	s.ResponseWriter.WriteHeader(code)
}

func RequestLogger(log *zap.Logger) Middleware {
	return func(next http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			start := time.Now()
			rec := &statusRecorder{ResponseWriter: w, status: http.StatusOK}
			next.ServeHTTP(rec, r)
			log.Info("http",
				zap.String("method", r.Method),
				zap.String("path", r.URL.Path),
				zap.Int("status", rec.status),
				zap.Duration("dur", time.Since(start)),
			)
		})
	}
}

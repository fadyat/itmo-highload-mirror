module github.com/fadyat/mlbd/hw2/api

go 1.26

require (
	github.com/caarlos0/env/v11 v11.4.0
	github.com/go-playground/form/v4 v4.3.0
	github.com/go-playground/validator/v10 v10.30.2
	github.com/google/renameio/v2 v2.0.2
	github.com/google/uuid v1.6.0
	github.com/jackc/pgx/v5 v5.9.2
	github.com/jmoiron/sqlx v1.4.0
	github.com/samber/lo v1.53.0
	go.uber.org/zap v1.27.1
	golang.org/x/sync v0.20.0
)

require (
	github.com/gabriel-vasile/mimetype v1.4.13 // indirect
	github.com/go-playground/locales v0.14.1 // indirect
	github.com/go-playground/universal-translator v0.18.1 // indirect
	github.com/jackc/pgpassfile v1.0.0 // indirect
	github.com/jackc/pgservicefile v0.0.0-20240606120523-5a60cdf6a761 // indirect
	github.com/jackc/puddle/v2 v2.2.2 // indirect
	github.com/leodido/go-urn v1.4.0 // indirect
	go.uber.org/multierr v1.11.0 // indirect
	golang.org/x/crypto v0.50.0 // indirect
	golang.org/x/mod v0.35.0 // indirect
	golang.org/x/sys v0.43.0 // indirect
	golang.org/x/telemetry v0.0.0-20260421165255-392afab6f40e // indirect
	golang.org/x/text v0.36.0 // indirect
	golang.org/x/tools v0.44.0 // indirect
	golang.org/x/vuln v1.3.0 // indirect
)

tool (
	golang.org/x/tools/cmd/deadcode
	golang.org/x/vuln/cmd/govulncheck
)

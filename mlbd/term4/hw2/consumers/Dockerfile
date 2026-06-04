FROM golang:1.26-alpine AS build
WORKDIR /src
COPY go.mod go.sum* ./
RUN go mod download || true
COPY . .
ARG TARGET
RUN CGO_ENABLED=0 go build -o /out/app ./cmd/${TARGET}

FROM gcr.io/distroless/static-debian12:nonroot
COPY --from=build /out/app /app
USER nonroot:nonroot
ENTRYPOINT ["/app"]

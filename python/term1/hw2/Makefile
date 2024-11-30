lint:
	@poetry run ruff check

fmt:
	@poetry run ruff check --fix
	@poetry run ruff format

run:
	@docker compose up -d

generate:
	@poetry run python main.py > artifacts/output.tex
	@docker compose exec -it tex bash -c "cd artifacts && pdflatex output.tex"

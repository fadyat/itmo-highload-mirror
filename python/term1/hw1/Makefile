test:
	@chmod +x artifacts/nl_test.sh && ./artifacts/nl_test.sh > artifacts/out/test.txt
	@chmod +x artifacts/tail_test.sh && ./artifacts/tail_test.sh >> artifacts/out/test.txt
	@chmod +x artifacts/wc_test.sh && ./artifacts/wc_test.sh >> artifacts/out/test.txt

samples:
	@poetry run python -m nl pyproject.toml > artifacts/out/nl.txt
	@poetry run python -m tail pyproject.toml > artifacts/out/tail.txt
	@poetry run python -m wc pyproject.toml > artifacts/out/wc.txt

lint:
	@poetry run ruff check

fmt:
	@poetry run ruff check --fix
	@poetry run ruff format
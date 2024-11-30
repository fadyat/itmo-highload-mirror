.PHONY: build

prepare:
	@python3 -m pip install --upgrade build
	@python3 -m pip install --upgrade twine

build:
	@python3 -m build

publish:
	@python3 -m twine upload dist/*

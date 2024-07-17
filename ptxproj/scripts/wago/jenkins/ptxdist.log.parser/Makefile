TEST_WORKSPACE=out/test-workspace/pfc
TEST_ARCHIVE=test/res/pfc-workspace.tar.gz
PYTHON3_VENV_BINARY = .venv/bin/python3
FILES_UNDER_INSPECTION = *.py lib/*.py test/*.py

PYTHON3 = pipenv run python3
AUTOPEP8 = $(PYTHON3) -m autopep8 $(FILES_UNDER_INSPECTION) --verbose
BLACK = $(PYTHON3) -m black lib $(FILES_UNDER_INSPECTION)
PYFLAKES = $(PYTHON3) -m pyflakes $(FILES_UNDER_INSPECTION)
PYLINT = $(PYTHON3) -m pylint $(FILES_UNDER_INSPECTION)
MYPY = $(PYTHON3) -m mypy $(FILES_UNDER_INSPECTION) --explicit-package-bases


default: all

.PHONY: print-%
print-%: ; @echo $($*)

$(PYTHON3_VENV_BINARY):
	pipenv sync

update:
	pipenv update

out/junit-reports:
	mkdir -p $@

check: $(PYTHON3_VENV_BINARY) $(TEST_WORKSPACE) | out/junit-reports
	$(PYTHON3) -m xmlrunner discover test --output out/junit-reports || true

all: $(PYTHON3_VENV_BINARY) expand
	$(PYLINT)
	$(PYFLAKES)
	$(MYPY)
	$(BLACK)
	$(AUTOPEP8)


expand: $(TEST_WORKSPACE)

$(TEST_WORKSPACE): | $(TEST_ARCHIVE)
	mkdir -p $(dir $@) \
	&& tar xf $(TEST_ARCHIVE) -C $(dir $@)

clean:
	rm -rf .venv/bin .venv/lib .venv/src

example-invocation:
	./ptxdist_parser.py -vv collect --buildfolder out/test-workspace/pfc --resolvefolder out/test-workspace/pfc out/test-workspace/pfc/ptxproj/platform-wago-pfcXXX/logfile out/all_issues.json

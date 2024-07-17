# Use python3.cit docker image. This image already has the packages libxslt-dev and libxml2-dev
# installed that are needed by unittest-xml-reporting.
PYTHON3_IMAGE=$(shell . /opt/docker-cli-scripts/docker-default-images.sh && \
echo $${DOCKER_REGISTRY}/$${DEFAULT_IMAGE_PYTHON_CIT_TRIGGER_2})

all:

# Run tests and place JUnit output in folder out
check: xml-reporting
	PYTHONPATH=$$PWD/out/pip \
	python3 -m xmlrunner discover test --output out

xml-reporting:
	PYTHON_3_IMAGE=$(PYTHON3_IMAGE) python3 -m pip install lxml==5.1.1 unittest-xml-reporting==3.2.0 --target out/pip

clean:
	rm -rf out test/__pycache__

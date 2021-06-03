X86_64_VARS := -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../bin

ARM64_VARS := -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../bin-arm64
ARM64_VARS += -DCMAKE_SYSTEM_PROCESSOR=aarch64
ARM64_VARS += -DCMAKE_C_COMPILER=${LINARO_PATH}/bin/aarch64-linux-gnu-gcc
ARM64_VARS += -DCMAKE_CXX_COMPILER=${LINARO_PATH}/bin/aarch64-linux-gnu-g++

all:
	if [ -f "${LINARO_PATH}/bin/aarch64-linux-gnu-g++" ]; then \
	    rm -r .cmake; mkdir -p .cmake bin-arm64; cd .cmake && cmake .. ${ARM64_VARS} && cd .. && make build; \
	else \
		rm -r .cmake; mkdir -p .cmake bin; cd .cmake && cmake .. ${X86_64_VARS} && cd .. && make build; \
	fi
.PHONY: all

build:
	cd .cmake && cmake --build .
.PHONY: build

clean:
	rm -rf .cmake bin bin-*
.PHONY: clean

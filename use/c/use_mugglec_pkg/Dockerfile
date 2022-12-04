ARG OS
ARG OS_VER
ARG MUGGLEC_VER

# step 1
FROM registry.mugglewei.com/mugglec:${MUGGLEC_VER}-${OS}${OS_VER} as mugglec

# step 2
FROM registry.mugglewei.com/${OS}-dev:${OS_VER} as builder

COPY --from=mugglec /usr/local/ /usr/local/
RUN mkdir -p /app/src/hello
RUN mkdir -p /app/src/hello/build
COPY ./CMakeLists.txt /app/src/hello/
COPY ./example /app/src/hello/example
WORKDIR /app/src/hello
RUN cmake \
	-S /app/src/hello \
	-B /app/src/hello/build \
	-DCMAKE_PREFIX_PATH=/usr/local \
	-DCMAKE_INSTALL_PREFIX=/usr/local \
	-DCMAKE_BUILD_TYPE=Release
RUN cmake --build /app/src/hello/build
RUN cmake --build /app/src/hello/build --target install

# step 2
FROM registry.mugglewei.com/${OS}-dev:${OS_VER}

COPY --from=builder /usr/local/ /usr/local/
ENV PATH="$PATH:/usr/local/bin"
WORKDIR /usr/local

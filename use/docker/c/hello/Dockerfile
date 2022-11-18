# multi-stage builds

ARG OS_VER

# step 1
FROM ubuntu-dev:${OS_VER} as builder

# build
RUN mkdir -p /app/src/hello
COPY ./CMakeLists.txt /app/src/hello
COPY ./example/ /app/src/hello/example
WORKDIR /app/src/hello
RUN mkdir -p build
RUN cmake -S /app/src/hello -B /app/src/hello/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/app
RUN cmake --build build --target install

# step 2
FROM ubuntu:${OS_VER}
RUN mkdir -p /app/bin
RUN mkdir -p /app/lib
WORKDIR /app/bin
COPY --from=builder /app/bin/ /app/bin
COPY --from=builder /app/lib/ /app/lib
ENV PATH="/app/bin:${PATH}"
ENV LD_LIBRARY_PATH="/app/lib"
CMD ["example_hello"]

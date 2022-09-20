FROM golang:1.19.1-alpine

# build
ENV GO111MODULE=on
ENV GOPROXY=https://goproxy.cn,direct
RUN mkdir -p /app/src
RUN mkdir -p /app/bin
COPY . /app/src
WORKDIR /app/src
RUN go build -o /app/bin/hello-service

# clean source
RUN rm -rf /app/src

# run
ENV service_number 0
EXPOSE 8080
WORKDIR /app/bin
CMD ./hello-service -bind=":8080" -number=$service_number

FROM hpb/hpb_ubuntu:22.04 as builder

ENV WORKING_DIR /app/src/hpb_docker_examples
RUN mkdir -p $WORKING_DIR
WORKDIR $WORKING_DIR
COPY modules ./modules
COPY build.sh ./
RUN mkdir -p ~/.hpb
RUN hpb dbsync
RUN git config --global http.https://github.com.proxy socks5://127.0.0.1:1080
RUN ./build.sh
RUN mv $WORKING_DIR/dist /app/dist

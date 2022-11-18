ARG OS_VER

FROM ubuntu:${OS_VER}

RUN apt-get update
RUN apt-get install build-essential -y
RUN apt-get install git -y
RUN apt-get install cmake -y

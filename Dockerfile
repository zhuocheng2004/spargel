
FROM ubuntu:latest
WORKDIR /spargel

RUN apt update && apt install -y cmake make ninja-build gcc g++ clang

COPY . .

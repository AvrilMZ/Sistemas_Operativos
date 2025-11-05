FROM ubuntu:24.04

RUN DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y make gcc build-essential
RUN apt-get install -y time python3 python3-pip

RUN pip install --break-system-packages termcolor pyyaml

WORKDIR /shell

COPY . .

RUN cc tests/reflector.c -o tests/reflector

RUN make clean && make -B -e SHELL_TEST=true

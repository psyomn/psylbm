# I'd rather use plain github actions, but valgrind is too old on
# ubuntu-latest.
FROM alpine:3.11.5

RUN apk update
RUN apk add gcc \
        sqlite sqlite-dev \
        openssl openssl-dev \
        libc-dev  \
        make cmake \
        valgrind \
        uncrustify

COPY . /app
WORKDIR /app

RUN mkdir build
WORKDIR /app/build

RUN cmake ..

RUN make

RUN CTEST_OUTPUT_ON_FAILURE=1 make test

RUN make check-format

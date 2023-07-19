FROM alpine:3.18
ARG APNGASM_VERSION=3.1.10
RUN apk add --virtual apngasm_build \
        git cmake libpng-dev boost-dev make g++ && \
    git clone https://github.com/apngasm/apngasm.git && \
    cd apngasm && \
    git checkout $APNGASM_VERSION && \
    mkdir build && \
    cd build && \
    cmake ../ && \
    make && \
    make install && \
    cd / && rm -rf /apngasm && \
    apk del apngasm_build && \
    apk add --virtual apngasm_deps \
        boost-program_options libpng

ENTRYPOINT ["apngasm"]

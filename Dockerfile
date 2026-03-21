FROM ubuntu:24.04 AS builder
LABEL authors="epsilon"

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    make \
    pkg-config \
    libglfw3-dev \
    libgl1-mesa-dev \
    libx11-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build -j$(nproc)

# -------

FROM ubuntu:24.04 AS runtime
LABEL authors="epsilon"

RUN apt-get update && apt-get install -y --no-install-recommends \
    libglfw3 \
    libgl1 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /app/build/falling_sand .
COPY --from=builder /app/shaders ./shaders

# Requires X11 forwarding:
#   docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix falling-sand
ENTRYPOINT ["./falling_sand"]
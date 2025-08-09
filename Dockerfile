# syntax=docker/dockerfile:1

# Build stage: compile bootloader and kernel, assemble disk image
FROM debian:stable-slim AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
    make nasm gcc binutils qemu-system-x86 xz-utils \
    libc6-dev gcc-multilib g++ g++-multilib \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .
RUN make clean && make

# Runtime stage: Node.js server with QEMU
FROM node:20-bullseye-slim AS runtime
RUN apt-get update && apt-get install -y --no-install-recommends qemu-system-x86 \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=build /app/dist /app/dist
COPY server /app/server
COPY frontend /app/frontend
ENV NODE_ENV=production
ENV PORT=8080
RUN npm ci --prefix server --omit=dev
EXPOSE 8080
CMD ["npm","start","--prefix","server"]


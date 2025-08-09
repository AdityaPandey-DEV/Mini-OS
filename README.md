# MiniOS — x86 toy OS with web terminal

MiniOS is a minimal x86 operating system with a custom bootloader and 32-bit C kernel. It integrates with a Node.js backend running QEMU in headless mode and a web frontend using xterm.js to provide an in-browser, real-time terminal. Each browser session runs its own QEMU instance.

Features
- Bootloader in x86 real mode, loads kernel from disk and switches to protected mode
- 32-bit kernel in C with:
  - VGA text output (colors)
  - IDT, PIC remap, timer (PIT) and keyboard interrupts
  - Simple shell with commands: help, echo, clear, about, mem, time, uptime, sysinfo, ls, cat
  - Built-in initrd-like files for ls/cat
  - Command history and tab completion
  - Custom kernel panic screen
- Web Terminal:
  - Node.js server spawns QEMU per connection (-nographic -serial stdio)
  - WebSockets stream QEMU I/O to xterm.js in the browser
  - Auto-resize, dark/light toggle, download OS image button
- Multi-user: separate QEMU process per WebSocket client
- Deployable to Fly.io (Dockerfile + fly.toml). Should also work on Railway/Render with Docker.

Quick start
- Requirements (local build):
  - macOS/Linux with: make, nasm, gcc (with -m32), binutils (ld with elf_i386), qemu-system-i386, Node.js 18+
  - If your host can’t do -m32, use Docker build (see below).

Build and run locally
- Build the OS image:
  make
- Run QEMU locally (directly):
  make run
- Start the web server (spawns QEMU per client):
  npm install --prefix server
  npm start --prefix server
  Then open http://localhost:8080/

Docker build (recommended)
- Build and run both OS and server via Docker:
  docker build -t minios:latest .
  docker run --rm -p 8080:8080 minios:latest
  Open http://localhost:8080/

Deploy to Fly.io
- Install flyctl, then:
  fly launch --copy-config --no-deploy
  fly deploy

Project Layout
- boot/        Bootloader (ASM)
- kernel/      Kernel (C/ASM, linker script)
- dist/        Built OS image artifacts
- server/      Node.js backend with WebSocket + QEMU control
- frontend/    xterm.js frontend (served by server)
- Dockerfile   Build and runtime container (includes QEMU)
- Makefile     Build toolchain for OS image
- fly.toml     Fly.io app configuration

License
- MIT


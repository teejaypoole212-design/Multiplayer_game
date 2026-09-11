# Multiplayer Game

A simple multiplayer game written in C++ with a host/client architecture for local network play.

## About

This project demonstrates basic multiplayer networking. It includes separate host and client entry points so two instances can connect on the same network.

This was a simple test game to see what I could do. The gameplay look is not complete, but it does function at a very basic level.

## Features

- Host / Client architecture
- Player movement & combat
- Collision detection
- Simple networking protocol
- Shared game world

## Structure

- `host/` – Host-side code
- `client/` – Client-side code
- Shared systems: player, world, collisions, protocol, etc.

## Building

```bash
make

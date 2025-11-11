# Pong
A terminal-based Pong implementation written in C using ncurses for rendering. The project supports local multiplayer, networked play, configurable game parameters, and has a basic menu-driven interface.

## Features
- Local 1v1 on a singular keyboard
- Online multiplay via a simple host/join model
- Configurable gameplay parameters (screen size, winning score, etc)

## Gamemodes

### Offline
Two-player match-off on the same machine. Controls:
- Player 1: `w / s`
- Player 2: `j / k`

### Online
Online play uses a host-client model:
1. One player selects `serve` to host a game, modifies the configuration to their liking, and starts the game.
2. The host is shown an IP address and port combo.
3. The other player selects `join` and enters that IP-port combo.

Configuration is authoritative on the host, but values which the client doesn't support are automatically clamped to supported values.
 
## Gameplay
Movement is handled by `w/s` or `j/k` depending on your side. The objective is to prevent the ball from passing your pad while trying to direct it past your opponent's.
Ball speed increases incrementally after each paddle hit and resets when a point is scored. The match is over when either player reaches the configured winning score.

Global controls:
- Pause: p
- Quit: q

In online matches, both players must confirm `resume` before the game continues.

## Configuration
The configuration menu allows adjusting gameplay parameters such as:

- Screen width / height
- Winning score
- Paddle size
- Initial ball speed

Each option includes an inline description. Values are capped to ensure the game remains playable. For example, the screen width cannot exceed the terminal width or drop below a minimum safe bound. 
Tall screen heights cause the scoreboard to fall back to single‑character digits instead of large ASCII art.

## Contributing
Developed by [code_radmark](https://github.com/code-redmark) and [tomScheers](https://github.com/tomScheers). You're more than welcome to submit a pull request or open an issue if you encounter problems or have suggestions.

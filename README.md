# Console Project
I am creating a game console that is supposed to resemble the Sega CD, PS2, and other consoles alike. 

It will read USB devices and run games stored on them. 

In the meantime, the user will be greeted with a UI, which will prompt them to "Enter a device/game".

### TODOs
- Make IPC server run on TCP (Unix domain socket)
- Give the game launching control/responsibility to a third entity
- Design the UI and control it via pygame (Python)
- Make all processes communicate via JSON (IPC)

### Ambitious goals
- Use input masking for games that only take keyboard input. E.g., transforming joystick input into keyboard keys.
- Use the systemd api to send information about input devices
- Expand the console to include multiple games and function like a home console. E.g., PS5 + Nintendo Switch etc.

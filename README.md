# Meowchkin

<p align="center">
  <img src="https://i.imgur.com/h88hI8y.png" />
</p>

*Free open-sourced online munchkin-inspired game for your linux/windows desktop, written in C++.*

## Features

- Multiplayer game by LAN for 2-4 players
- Munchkin-like gameplay
- Statistics of card is tracked and saved locally
- Text chat
- Creation of your own cards ([not user-friendly](#card-creation))

## Screenshots
<p align="center">
  <img src="https://i.imgur.com/6bLDO42.png" width=80% />
</p>
<p align="center">
  <img src="https://i.imgur.com/1cAZJLj.png" width=80% />
</p>
<p align="center">
  <img src="https://i.imgur.com/Mhyy9nv.png" width=80% />
</p>

## Dependencies

Meowchkin uses a number of open source projects to work properly:

- [boost](https://www.boost.org/) - Multiplayer handling and other cool features.
- [raylib-cpp](https://github.com/RobLoach/raylib-cpp) - For awesome 2D graphics.
- [raygui](https://github.com/raysan5/raygui) - For easier UI development.
- [sqlite3](https://github.com/sqlite/sqlite) - To track your statistics.
- [nlohman](https://github.com/nlohmann/json) - For convinient work with JSON's.

## Installation

### Release binaries
_TODO_

### Building from source

#### Linux
Install dependencies (ubuntu commands):
```bash
sudo apt update && sudo apt upgrade
sudo apt install git build-essential libboost-all-dev sqlite libsqlite3-dev
```

Clone repo & run build system:
```bash
git clone https://github.com/mmalofeev/Meowchkin.git
cd Meowchkin/build/
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON
make
```

And wait. It can take a few minutes

Launch:
```bash
./meowchkin
```

#### Windows
_TODO_


## Game rules
[Here!](RULES.MD)

## Card creation
You can add card without changing source code and recompliling whole project. To make it, describe card in it's JSON file.

_TODO_: straightforward instructions

Example of card description:

<img src=https://i.imgur.com/JCreylI.png width=600px/>

## Team
 - [Gusarov Artyom](https://github.com/Artyom-Gusarov)
 - [Dorosev Anton](https://github.com/mrprizmo)
 - [Kort Pavel](https://github.com/pppppptttttt)

Under the guidance of [Malofeev Mikhail](https://github.com/mmalofeev)

## License

MIT



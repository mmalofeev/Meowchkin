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
- Creation of your own cards (not user-friendly)

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
git clone git@github.com:mmalofeev/Meowchkin.git
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
Each player starts with 1 level and 4 cards. The goal of the game is to reach 10 levels. The last level can only be achieved by killing a monster.

### Cards

#### Spells
Cards that have an effect on a monster or player and have a limited duration. Spells can be played in any phase of the game.

#### Equipment
Equipment gives bonuses to the player's attack and defense, as well as other bonuses. Each piece of equipment has a cost. Each player can wear only one helmet, one armor, one pair of shoes, and take two hand items (or one two-handed item).
You can remove equipment at any time, but you can only put it on at the beginning or end of your turn.

#### Monsters
Monsters have strength and nastiness. If a manchkin loses a battle to a monster and fails to escape, they must perform the nastiness.

### Phases of the Game
#### Preparation
You prepare for your journey, thinking about your strategy. You play cards from your hand that you think are necessary.
#### Opening the Door
Draw one card from the dungeon deck. If it's a monster, go to the battle phase. If it's a spell, apply it to the player. Otherwise, add it to your hand.
#### Battle
By default, a manchkin must have more strength than the combined strength of the monsters to win. If you win, you gain 1 level (2 if indicated on the card) and the treasure indicated on the card.
#### Escape
If you lose, you can try to avoid the nastiness by rolling a 5 or higher on a dice. If you fail to escape and the monster is still pursuing you, you must perform the nastiness.
Searching for Trouble:
If you haven't encountered monsters when opening the door, you can play a monster card if you have one and fight it. If you don't like fighting monsters, you can simply take two cards from the dungeon deck.
#### End of Turn
All goes back to normal. You can do the same things you did during preparation, and when you get bored, you can end your turn.


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



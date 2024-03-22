# Meowchkin

## Quick overview
For now you can just drop on board cards from your hand. Another player will see them

## Build
Build and launch your executable next to `bin` directory, e.g. in `build`:
```bash
cd build
cmake -DBUILD_SHARED_LIBS=ON ..  # i think this is important
make
```
And make sure you have boost

## Launch(work in progress)
For now, on localhost only, for 2 clients.
On linux:
1) run server
```bash
cd build
./meowchkin host
```
2) run clients(2 for now)
```bash
cd build
./meowchkin client
```

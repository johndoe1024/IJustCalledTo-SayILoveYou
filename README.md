# Imaginary Player

## Build

### Dummy (no mp3 decoding support)
```bash
git clone https://github.com/johndoe1024/IJustCalledTo-SayILoveYou.git iplayer
mkdir iplayer/build
cd iplayer/build
cmake .. # use -DOPTION_IPLAYER_ENABLE_LOG=ON for verbose mode
make
```

### IPlayer with mp3 support (libmad)
```bash
apt install libmad0-dev
git clone https://github.com/johndoe1024/IJustCalledTo-SayILoveYou.git iplayer
mkdir iplayer/build
cd iplayer/build
cmake -DOPTION_IPLAYER_DECODER_MAD=ON ..
make
```

## Running iplayer
```bash
cd iplayer/build
./iplayer
```

```
# add a dummy file
>>> add_track foobar

# add all mp3 of a directory
>>> add_track file:///home/user/music

```

## Running test suite
```bash
cd iplayer/build
ctest
```

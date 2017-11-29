# Imaginary Player

## Build
```bash
git clone https://github.com/johndoe1024/IJustCalledTo-SayILoveYou.git iplayer
mkdir iplayer/build
cd iplayer/build
cmake .. # use -DOPTION_IPLAYER_ENABLE_LOG=ON for verbose mode
make
```

## Running iplayer
```bash
cd iplayer/build
./iplayer
```

## Running test suite
```bash
cd iplayer/build
ctest
```

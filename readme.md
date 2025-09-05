# EventExchange - Setup

## Dependencies

### Ubuntu
```bash
sudo apt-get install build-essential procps curl file git
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
export CMAKE_PREFIX_PATH="/home/linuxbrew/.linuxbrew:$CMAKE_PREFIX_PATH"
brew install gcc@14
export CC=/home/linuxbrew/.linuxbrew/bin/gcc-14 && export CXX=/home/linuxbrew/.linuxbrew/bin/g++-14
```
Follow instructions to get brew into path.

### macOS & Ubuntu
```bash
brew install boost crow sqlitecpp catch2
```

## Build
```bash
./compile.sh
```

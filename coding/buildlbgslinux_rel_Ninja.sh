# apt install ninjia-build -y

cmake -S "./" -B "../build" -DCMAKE_BUILD_TYPE=Release -DENABLE_DEBUG_LOG=OFF -DENABLE_DEBUG_MODE=OFF -DENABLE_UNITEST=OFF -G Ninja $*

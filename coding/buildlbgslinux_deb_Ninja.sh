# apt install ninjia-build -y

cmake -S "./" -B "../build" -DCMAKE_BUILD_TYPE=Release -DENABLE_DEBUG_LOG=ON -DENABLE_DEBUG_MODE=ON -DENABLE_UNITEST=ON -G Ninja $*

#!/bin/bash
echo "server loading"
nohup ../build/exe/proxyserver > ../build/exe/err_proxyserver.log 2>&1 &
sleep 1s
nohup ../build/exe/gateserver > ../build/exe/err_gateserver.log 2>&1 &
echo "server start succ!!!!!"

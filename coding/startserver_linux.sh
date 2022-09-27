#!/bin/bash
echo "server loading"
echo "start proxyserver......"
nohup ./exe/proxyserver > ../build/exe/err_proxyserver.log 2>&1 &
echo "proxyserver started!!!"
sleep 1s

echo "start gateserver......"
nohup ./exe/gateserver > ../build/exe/err_gateserver.log 2>&1 &
echo "gateserver started!!!"
sleep 1s

echo "start logicserver......"
nohup ./exe/logicserver > ../build/exe/err_logicserver.log 2>&1 &
echo "logicserver started!!!"
sleep 1s

echo "start cacheserver......"
nohup ./exe/cacheserver > ../build/exe/err_cacheserver.log 2>&1 &
echo "cacheserver started!!!"

echo "start dbserver......"
nohup ./exe/dbserver > ../build/exe/err_dbserver.log 2>&1 &
echo "dbserver started!!!"


echo "all server start succ!!!!!"


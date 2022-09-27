#!/bin/bash

echo "proxyserver stoping...."
pkill proxyserver 

echo "gateserver stoping...."
pkill gateserver

echo "logicserver stoping...."
pkill logicserver

echo "cacheserver stoping...."
pkill cacheserver

echo "dbserver stoping...."
pkill dbserver

echo "stop all server succ!!"

@echo off

taskkill /f /im logicserver.exe
taskkill /f /im gateserver.exe
taskkill /f /im proxyserver.exe
taskkill /f /im cacheserver.exe
taskkill /f /im dbserver.exe

pause
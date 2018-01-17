#!/bin/bash

exec ./login-server > ./logs/login-server.txt &
exec ./char-server > ./logs/char-server.txt &
exec ./map-server > ./logs/map-server.txt &
sleep infinity

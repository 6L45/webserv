#!/bin/bash

make client_simulator

./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&
./client_simulator client_todo/client1.cl > clien1_track&
./client_simulator client_todo/client2.cl > client2_track&

wait

make fclean
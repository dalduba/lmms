#!/bin/bash

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/i386-linux-gnu:/usr/lib/x86_64-linux-gnu/ C_INCLUDE_PATH=$C_INCLUDE_PATH:/usr/include/x86_64-linux-gnu/c++/4.8:/usr/include/x86_64-linux-gnu/ CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/usr/include/x86_64-linux-gnu/c++/4.8:/usr/include/x86_64-linux-gnu/ make -j4

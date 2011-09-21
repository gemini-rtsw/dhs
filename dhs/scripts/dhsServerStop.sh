#!/bin/bash

echo "Stoping DHS services:"
/etc/init.d/dhs stop
/etc/init.d/imp_master stop

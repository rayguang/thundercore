#!/bin/bash

TRADEHOME_DIR=/home/ray/dev/trading_execution
MIDDLEWARE_DIR=${TRADEHOME_DIR}/middleware
TRADING_DIR=${TRADEHOME_DIR}/src/trading

echo "************************************"
echo "Stop all supervisor trading services"
echo "************************************"

# The stop and start order MUST be respected to avoid
# losing messages!!!
echo "Stopping middlware KMS ......"
sudo supervisorctl stop trading_middlelayer_kms

echo "Stoping middlware trading request relay ......"
sudo supervisorctl stop trading_middlelayer_guardian

echo "Stopping trading core ......"
sudo supervisorctl stop trading_core


echo "Starting crypto trading core ......"
sudo supervisorctl start trading_core

echo "Starting middleware trading request relay ......"
sudo supervisorctl start trading_middlelayer_guardian

echo "Starting middleware KMS ......"
sudo supervisorctl start trading_middlelayer_kms

#################################
# Ray Guang 
# June 2019
# This program gets active exchange API ID from the backend
# The ID is dumped to a text file every 10 seconds

################################

import pysher
import sys
import time
import os
import json
import logging
from logging.handlers import TimedRotatingFileHandler

import subprocess

import mysql.connector
from mysql.connector import Error

from dotenv import load_dotenv
from pathlib import Path
env_path = Path('..')/'.env'
load_dotenv(dotenv_path=env_path)

node_bin    = '/usr/bin/node'
node_cmd    = '/home/ray/dev/trading_execution/middleware/middleware_trading_multichannel.js'
kill_cmd    = '/bin/kill'
pgrep_cmd   = '/usr/bin/pgrep'
check_param = ' -f ' + node_cmd

def init_proc():
    print("Detecting if any running process of ", node_cmd)
    running_pid = subprocess.run([pgrep_cmd, '-f', node_cmd], stdout=subprocess.PIPE)
    node_pid    = running_pid.stdout.decode('utf-8').splitlines()
    for pid in node_pid:
        print ("Terminating process {}......".format(pid))
        try:
            retcode = subprocess.call([kill_cmd, str(pid)])
            if retcode == 0:
                print("Process {} killed successfully.".format(pid))
            else:
                print("Process {} failed to be terminated.".format(pid))
        except Exception as e:
            print(e)


# Add a logging handler so we can see the raw communication data
logger = logging.getLogger()
logger.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
fh = TimedRotatingFileHandler(os.getenv("MIDDLE_GUARDIAN_LOG"),
        when="m",
        interval=1,
        backupCount=5)
fh.setFormatter(formatter)
logger.addHandler(fh)

# My SQL Credentials--------------------------------------------------------
backend_user        = os.getenv("BACK_DBUSER")
backend_password    = os.getenv("BACK_DBPASSWORD")
backend_database    = os.getenv("BACK_DBNAME")
backend_host        = os.getenv("BACK_DBHOST") #Prod

time_format = '%Y-%m-%d %H:%M:%S'

print ("*******************************************************")
print ("************* Crypto GUARDIAN Middleware **************")
print ("*******************************************************")

uid_list_old = []
uid_list_new = []
proc_dict = {}

init_proc()


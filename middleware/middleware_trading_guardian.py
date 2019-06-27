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
print(check_param)

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

def getActiveAPIUid():
    logging.info("****** Fetch active API key from backend ******")
    try:
        conn_back = mysql.connector.connect(host=backend_host,
                                       database=backend_database,
                                       user=backend_user,
                                       password=backend_password )
        if conn_back.is_connected():
            logging.info("*** Connected to trading DB successfully ***")
        cursor = conn_back.cursor(buffered=True)

        # Retrieve info from DB
        sql_select_query = """SELECT distinct user_id FROM exchange_API """
        cursor.execute(sql_select_query)
        rows = cursor.fetchall()
        logging.info ('Total rows: '+ str(cursor.rowcount))
        
        uid_list = []

        if cursor.rowcount >= 1:
            for row in rows:
                #logging.info(row)
                _uid    = row[0]
                if ( not _uid ):
                    logging.error("ERROR: uid is empty.")
                    return
                else:
                    uid_list.append(_uid)
            return uid_list
        else:
            logging.info ('No rows found returned!' + str(cursor.rowcount))
            return
    except Error as e:
        logging.error(e)
    finally:
        conn_back.close()
        logging.info("*** Connection to trading DB is closed. ***")

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

while True:
    # Fetch active API user ID
    print("Starting to fetch user ID from API table...")
    uid_list_new = getActiveAPIUid()
    file = open('nodejs_uidpid.txt', 'w')

    # Check if any new API has been added, if yes, start pusher listener
    for uid in uid_list_new:
        if uid not in uid_list_old:
            print("Starting new node process uid: ", uid)
            command = [node_bin, node_cmd, str(uid)]
            print("Starting process: ", command)
            pid = subprocess.Popen(command).pid
            proc_dict[str(uid)] = pid
            print("Process started: uid-{}  pid-{}".format(id, pid))
            file.write("%s %s\n" % (str(uid), str(pid)))
    
    file.close() 

    # Check if any API has been deleted, if yes, kill existing pusher listener
    for uid in uid_list_old:
        if uid not in uid_list_new:
            print("Killing pusher listener to uid: ", uid)
            print("Killing process: uid-{} pid-{}".format(uid, proc_dict[uid]) )
            retcode = subprocess.call([kill_cmd, str(proc_dict[uid])])
            if retcode == 0:
                print("Process {} killed successfully.".format(proc_dict[uid]))
            else:
                print("Process {} failed to be terminated.".format(proc_dict[uid]))
    
    # Save uid list as old list and empty current uid list
    uid_list_old = uid_list_new
    uid_list_new = []

    # Wait x seconds before next scan for new IDs
    time.sleep(10)

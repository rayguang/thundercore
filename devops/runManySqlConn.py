import os
import sys
import time
from datetime import datetime

import mysql.connector
from mysql.connector import Error

from dotenv import load_dotenv
from pathlib import Path
env_path = Path('.')/'.env'
load_dotenv(dotenv_path=env_path)

# frontend DB parameters
frontend_user       = os.getenv("FRONT_DBUSER")
frontend_password   = os.getenv("FRONT_DBPASSWORD")
frontend_database   = os.getenv("FRONT_DBNAME")
frontend_host       = os.getenv("FRONT_DBHOST") #Prod

runtime=[]

def sqlConnTest(iterator):
    try:
            conn_front = mysql.connector.connect(host=frontend_host,
                                        database=frontend_database,
                                        user=frontend_user,
                                        password=frontend_password )
            if conn_front.is_connected():
                print("*** Connected to front DB successfully ***")
                cursor = conn_front.cursor(buffered=True)

                # Retrieve info from DB
                sql_select_query = """select count(*) from cryptote_db2.trends"""
                start = time.time()*1000
                cursor.execute(sql_select_query)
                rows = cursor.fetchmany()
                end = time.time()*1000
                runtime.append(end-start)
                print('Total rows: '+ str(cursor.rowcount))
                print('Execution time: {}'.format(end-start))
            else:
                print ('No rows found or more than one row found!' + str(cursor.rowcount))
    except Error as e:
            print(e)
    finally:
            conn_front.close()
            print("*** Connection to front DB is closed. ***")


def main(*args, **kwargs):
    for i in range(2):
        sqlConnTest(i)
    print("Total runtime: {}, average runtime: {}".format(sum(runtime), sum(runtime)/len(runtime)) )
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
~                                       

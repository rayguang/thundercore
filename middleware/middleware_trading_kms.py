#################################
# Ray Guang 
# April 2019
################################

import pysher
import sys
import time
import datetime
import os
import json
import logging
from logging.handlers import TimedRotatingFileHandler

import base64
import hashlib
from Crypto import Random
from Crypto.Cipher import AES

import mysql.connector
from mysql.connector import Error

from dotenv import load_dotenv
from pathlib import Path
env_path = Path('..')/'.env'
load_dotenv(dotenv_path=env_path)

# Add a logging handler so we can see the raw communication data
logger = logging.getLogger()
logger.setLevel(logging.INFO)
formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
#ch = logging.StreamHandler(sys.stdout)
#root.addHandler(ch)
fh = TimedRotatingFileHandler(os.getenv("MIDDLE_KMS_LOG"),
        when="m",
        interval=1,
        backupCount=5)
fh.setFormatter(formatter)
logger.addHandler(fh)

appkey = os.getenv("APP_KEY_TRADING_PRD")
pusher = pysher.Pusher(appkey, cluster=os.getenv("APP_CLUSTER_TRADING_PRD"))

# To move this into .env
# My SQL Credentials--------------------------------------------------------
frontend_user       = os.getenv("FRONT_DBUSER")
frontend_password   = os.getenv("FRONT_DBPASSWORD")
frontend_database   = os.getenv("FRONT_DBNAME")
frontend_host       = os.getenv("FRONT_DBHOST") #Prod

backend_user        = os.getenv("BACK_DBUSER")
backend_password    = os.getenv("BACK_DBPASSWORD")
backend_database    = os.getenv("BACK_DBNAME")
backend_host        = os.getenv("BACK_DBHOST") #Prod

time_format = '%Y-%m-%d %H:%M:%S'

'''
Define function to retrieve the API key/secret from 
front-end server
1) retrieve encrypted key/secret once newuser creation message received
2) save to trading engine DB or separate DB
3) delete key/secret from front-end DB once 2) succeed 
'''
class AESCipher(object):
    def __init__(self, key):
        self.bs = 16 # key and secret phrase 32 bytes
        self.key = key[:16] # key must be 16 bytes
        #print("Init parameter: blocksize {}".format(self.blocksize))
        #print("[key(to encrypt)]:\t", self.key)

    def encrypt(self, plaintext):
        if plaintext is None or len(plaintext) == 0:
            raise NameError("No plaintext is given to encrypt!!!")
        #print ("plaintext before pad: ", plaintext)
        plaintext = self._pad(plaintext)
        #print ("plaintext after pad: ", plaintext)
        #iv = Random.new().read(AES.block_size)  #16 bytes
        iv = os.urandom(self.bs) 
        '''
        print("\n******** Function ecrypt() ********")
        print("AES.block_size: ", AES.block_size)
        print ("[iv]: ", iv)
        print ("[iv(encoded)]: ", base64.b64encode(iv))
        '''
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        ciphertext = cipher.encrypt(plaintext)
        ciphertext_iv = iv+ciphertext
        encoded = base64.b64encode(iv+ciphertext)
        '''
        print ("\n[encrypted]: ", ciphertext)
        print ("\n[encrypted(encoded): ", base64.b64encode(ciphertext))
        print ("\n[iv+encrypted]: ",(iv+ciphertext))
        print ("\n[encoded]: {}\n".format(encoded))
        print ("\b[return]: ", str(encoded, 'utf-8'))
        '''
        return str(encoded, 'utf-8')


    def decrypt(self, enc):
        if enc is None or len(enc) == 0:
            raise NameError("No enc is given to decrpt !!!")
        decoded = base64.b64decode(enc)
        iv = decoded[:16]
        cipher = AES.new(self.key, AES.MODE_CBC, iv)
        decrypted = cipher.decrypt(decoded[16:])
        '''
        print("\n******** Function decrypt() ********")
        print("[ciphertext]: ", enc)
        print ("\n[iv]: ", iv)
        print ("\n[key]: ",self.key)
        print ("\n[decoded]: ", decoded)
        print ("\n[decrypted]:", decrypted)
        '''
        return str(self._unpad(decrypted), 'utf-8')

    def _pad(self, s):
        return s + (self.bs - len(s) % self.bs) * chr(self.bs - len(s) % self.bs)

    def _unpad(self, s):
        return s[:-ord(s[len(s)-1:])]

# def insert_exchangeapi(user_id, excahnge_name, exchange_id, apikey, apisecret, create_time):

# def update_exchangeapi(user_id, exchange_name, exchange_id, apikey, apisecret, update_time):

def search_newkey():
    logging.info("Checking if any new API key is created...")

    # *************************************************************
    # Request to ADD new API
    # *************************************************************
    # Setup DB conneciton
    global conn
    global cursor
    
    logging.info("[Search New API]: Connecting to front DB ...... ")
    try:
        rows= ""
        conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
        if conn_front.is_connected():
            logging.info("[Search New API]: *** Connected to front DB successfully ***")
        cursor = conn_front.cursor(buffered=True)

        # Retrieve info from DB
        sql_select_query = """select user_id, exchange_name, apikey, apisecret, misc from exchange_api_staging """
        cursor.execute(sql_select_query)
        rows = cursor.fetchmany(1)
        logging.info ('[Search New API]: Total rows: '+ str(cursor.rowcount))

        if cursor.rowcount >= 1:
            for row in rows:
                print(row)
                _uid            = row[0]
                _exchange_name  = row[1]
                _key            = row[2]
                _secret         = row[3]
                _misc           = row[4]
                if ( not _secret or not _key ):
                    logging.error("ERROR: key or secret is empty.")
                    return
        else:
            logging.info ('[Search New API]: No rows found !' + str(cursor.rowcount))
            return

    except Error as e:
        logging.error(e)
    finally:
        conn_front.close()
        logging.info("[Search New API]:  Connection to front DB is closed.")
    

    # Save API pair to DB
    try:
        conn_back = mysql.connector.connect(host=backend_host,
                                       database=backend_database,
                                       user=backend_user,
                                       password=backend_password )
        if conn_back.is_connected():
            logging.info("[Search New API]: Connected to backend DB successfully ... saving API")
        cursor_back = conn_back.cursor(prepared=True)

        
        #print("Data before encryption: ", _secret)
        #print("key: ",_key)
        #print("secret: ", _secret)
        try:
            cipher = AESCipher(_key)
            secret_encrypted = cipher.encrypt(_secret)
        except Error as e:
            logging.error(e)
        
        
        #print("Data after encryption: ", secret_encrypted)
        #print("Data after decryption: ", cipher.decrypt(secret_encrypted))
        #print("misc: ", _misc )

        userId      = _uid
        exchange    = _exchange_name 

        # Save API to backend DB
        sql_delete_query            = """ DELETE FROM `exchange_API` where user_id = %s and exchange_name = %s  """
        sql_insert_query            = """ INSERT INTO `exchange_API` (`user_id`, `exchange_name`, `apikey`, `apisecret`, `misc`) VALUES (%s, %s, %s, %s, %s)"""
        sql_insert_staging_query    = """ INSERT INTO `exchange_API_staging` (`user_id`, `exchange_name`, `apikey`, `apisecret`, `misc`) VALUES (%s, %s, %s, %s, %s)"""
        delete_tuple            = (userId, exchange)
        insert_tuple            = (userId, exchange, _key, secret_encrypted, _misc)
        insert_staging_tuple    = (userId, exchange, _key, _secret, _misc)

        cursor_back.execute(sql_delete_query, delete_tuple)
        conn_back.commit()
        logging.info("Deleting existing records for userId " + str(userId) + " " +str(_exchange_name))

        cursor_back.execute(sql_insert_staging_query, insert_staging_tuple)
        conn_back.commit()
        logging.info("Inserting exchange API to exchange_API_staging successfully.")

        cursor_back.execute(sql_insert_query, insert_tuple)
        conn_back.commit()
        logging.info("Inserting exchange API to exchange_API successfully.")

        # Delete API in front DB staging
        try:
            conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
            if conn_front.is_connected():
                print("*** Open connection to front-end DB -- delete front-end staging API ***")
            cursor_front = conn_front.cursor(buffered=True)

            sql_delete_front_query  = """DELETE from `exchange_api_staging` where user_id = %s and exchange_name = %s  """
            delete_front_tuple      = (userId, exchange)
            cursor_front.execute(sql_delete_front_query, delete_front_tuple)
            conn_front.commit()
            logging.info("User API delete from frontend successfully.")
        except Error as e:
            logging.error(e)
        finally:
            conn_back.close()
            logging.info("*** Close connection to front-end DB - delete front-end staging API  ***")

        # Update API status in front DB 
        try:
            conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
            if conn_front.is_connected():
                print("*** Connected to front DB successfully -- update API in frontend DB API table ***")
            cursor_front = conn_front.cursor(buffered=True)

            sql_delete_front_query  = """DELETE from `exchange_api` where user_id = %s and exchange_name = %s  """
            delete_front_tuple      = (userId, exchange)
            cursor_front.execute(sql_delete_front_query, delete_front_tuple)
            conn_front.commit()
            logging.info("User API delete from frontend successfully.")

            sql_insert_front_query  = """ INSERT INTO `exchange_api` (`user_id`, `exchange_name`, `apikey`, `apisecret`, `status`) VALUES (%s, %s, %s, %s, %s)  """
            insert_tuple            = (userId, exchange, "***", "***", "ACTIVE")
            cursor_front.execute(sql_insert_front_query, insert_tuple)
            conn_front.commit()
        except Error as e:
            logging.error(e)
        finally:
            conn_back.close()
            logging.info("*** Connection to front DB closed - after API insertion to backend ***")
            
    except Error as e:
        logging.error(e)
    finally:
        conn_back.close()
        logging.info("Connection to backend DB is closed.")

def search_deletekey():
    logging.info("Checking if any request to DELETE API key ...")

    # *************************************************************
    # Request to DELETE existing API
    # *************************************************************
    logging.info("[DELETE API]: Connecting to front DB ...... ")
    try:
        rows= ""
        conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
        if conn_front.is_connected():
            logging.info("*** [DELETE API]: Connected to front DB successfully ***")
        cursor = conn_front.cursor(buffered=True)

        # Retrieve info from DB
        status = "DELETING"
        sql_select_query = """ select user_id, exchange_name, status from exchange_api where status = %s """
        cursor.execute(sql_select_query, (status,))
        rows = cursor.fetchmany(1)
        print( '[DELETE API]: Total rows: '+ str(cursor.rowcount))

        if cursor.rowcount == 1:
            for row in rows:
                print(row)
                _uid            = row[0]
                _exchange_name  = row[1]
                _stat           = row[2]

                userId          = _uid
                exchange        = _exchange_name
            print("Found API pending to be deleted: " + str(userId) + " " + str(exchange))

            # Save API pair to DB
            try:
                conn_back = mysql.connector.connect(host=backend_host,
                                            database=backend_database,
                                            user=backend_user,
                                            password=backend_password )
                if conn_back.is_connected():
                    logging.info("[DELETE API]: Connected to backend DB successfully ... saving API")
                cursor_back = conn_back.cursor(prepared=True)
                

                # Delete API to backend DB
                sql_delete_query            = """ DELETE FROM `exchange_API` where user_id = %s and exchange_name = %s """
                delete_tuple                = (userId, exchange)
                
                cursor_back.execute(sql_delete_query, delete_tuple)
                conn_back.commit()
                logging.info("[DELETE API]: Deleting existing records for userId " + str(userId) + " " +str(_exchange_name))

                # Update API status in front DB 
                try:
                    conn_front = mysql.connector.connect(host=frontend_host,
                                            database=frontend_database,
                                            user=frontend_user,
                                            password=frontend_password )
                    if conn_front.is_connected():
                        print("*** [DELETE API]: Connected to front DB successfully -- update API status frontend DB API table ***")
                    cursor_front = conn_front.cursor(buffered=True)
                    
                    status = "DELETED"
                    sql_update_front_query  = """ UPDATE `exchange_api` set status = %s where user_id = %s and exchange_name = %s """
                    update_front_tuple      = (status, userId, exchange)
                    cursor_front.execute(sql_update_front_query, update_front_tuple)
                    conn_front.commit()
                    logging.info("[DELETE API]: Update frontend API status to DELETED.")

                except Error as e:
                    logging.error(e)
                finally:
                    conn_back.close()
                    logging.info("*** Connection to front DB closed - after API insertion to backend ***")
                    
            except Error as e:
                logging.error(e)
            finally:
                conn_back.close()
                logging.info("Connection to backend DB is closed.")

        else:
            logging.info ('No rows found or more than one row found!' + str(cursor.rowcount))
            return

    except Error as e:
        logging.error(e)
    finally:
        conn_front.close()
        logging.info("*** Connection to front DB is closed. ***")
    


def  func_encrypt(message):
    logging.info("Received message: "+ message)

    # Parsing json message
    msg_json        = json.loads(message)
    requestType     = msg_json['requestType']
    subType         = msg_json['subType']
    userId          = msg_json['userID']
    exchange        = msg_json['exchange']
    #print("Message received requestType: {}, subType: {}, userId: {}, exchange: {}".format(requestType, subType, userId, exchange))
    
    # *************************************************************
    # Initial sanity check
    # *************************************************************
    if (requestType != "ExchangeAPI" or subType not in ["NEW", "DELETE"]):
        logging.error("requestType or subType is UNKNOWN!")
        return

    if ( not userId or not exchange):
        logging.error("userId or exchange is NULL!")
        return

    # *************************************************************
    # Request to DELETE existing API
    # *************************************************************
    if (requestType == "ExchangeAPI" and subType == "DELETE"):
        logging.info("****** Requesting to DELETE API keys ******") 
        try:
            conn_back_d = mysql.connector.connect(host=backend_host,
                                       database=backend_database,
                                       user=backend_user,
                                       password=backend_password )
            if conn_back_d.is_connected():
                logging.info("Connected to backend DB successfully ... Deleting API ")
            cursor_back_d = conn_back_d.cursor(prepared=True)
            
            sql_delete_d_query = """ DELETE FROM `exchange_API` where user_id = %s and exchange_name = %s  """
            delete_d_tuple     = (userId, exchange)
            cursor_back_d.execute(sql_delete_d_query, delete_d_tuple)
            conn_back_d.commit()
            logging.info("Deleting backend API records for userId " + str(userId)+exchange)
        except Error as e:
            logging.error(e)
        finally:
            conn_back_d.close()
            logging.info("Connection to backend DB is closed ... Deleted API")
            return


    # *************************************************************
    # Request to ADD new API
    # *************************************************************
    # Setup DB conneciton
    global conn
    global cursor
    
    logging.info("****** Requesting to add NEW API keys ******")
    logging.info("Connecting to front DB ...... ")
    try:
        conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
        if conn_front.is_connected():
            logging.info("*** Connected to front DB successfully ***")
        cursor = conn_front.cursor(buffered=True)

        # Retrieve info from DB
        sql_select_query = """select user_id, exchange_name, exchange_id, apikey, apisecret, misc from exchange_api_staging where user_id = %s and exchange_name = %s """
        cursor.execute(sql_select_query, (userId, exchange))
        rows = cursor.fetchmany()
        logging.info ('Total rows: '+ str(cursor.rowcount))

        if cursor.rowcount == 1:
            for row in rows:
                #logging.info(row)
                _exchange_id    = row[2]
                _key            = row[3]
                _secret         = row[4]
                _misc           = row[5]
                if ( not _secret or not _key ):
                    logging.error("ERROR: key or secret is empty.")
                    return
        else:
            logging.info ('No rows found or more than one row found!' + str(cursor.rowcount))
            return

    except Error as e:
        logging.error(e)
    finally:
        conn_front.close()
        logging.info("*** Connection to front DB is closed. ***")

    # Save API pair to DB
    try:
        conn_back = mysql.connector.connect(host=backend_host,
                                       database=backend_database,
                                       user=backend_user,
                                       password=backend_password )
        if conn_back.is_connected():
            logging.info("Connected to backend DB successfully ... saving API")
        cursor_back = conn_back.cursor(prepared=True)

        
        #print("Data before encryption: ", _secret)
        #print("key: ",_key)
        #print("secret: ", _secret)
       
        cipher = AESCipher(_key)
        secret_encrypted = cipher.encrypt(_secret)
        
        
        #print("Data after encryption: ", secret_encrypted)
        #print("Data after decryption: ", cipher.decrypt(secret_encrypted))
        

        # Save API to backend DB
        sql_delete_query            = """ DELETE FROM `exchange_API` where user_id = %s and exchange_name = %s  """
        sql_insert_query            = """ INSERT INTO `exchange_API` (`user_id`, `exchange_name`, `exchange_id`, `apikey`, `apisecret`, `misc`) VALUES (%s, %s, %s, %s, %s, %s)"""
        sql_insert_staging_query    = """ INSERT INTO `exchange_API_staging` (`user_id`, `exchange_name`, `exchange_id`, `apikey`, `apisecret`, `misc`) VALUES (%s, %s, %s, %s, %s, %s)"""
        delete_tuple            = (userId, exchange)
        insert_tuple            = (userId, exchange, _exchange_id, _key, secret_encrypted, _misc)
        insert_staging_tuple    = (userId, exchange, _exchange_id, _key, _secret, _misc)

        cursor_back.execute(sql_delete_query, delete_tuple)
        conn_back.commit()
        logging.info("Deleting existing records for userId " + str(userId))

        cursor_back.execute(sql_insert_staging_query, insert_staging_tuple)
        conn_back.commit()
        logging.info("Inserting exchange API to exchange_API_staging successfully.")

        cursor_back.execute(sql_insert_query, insert_tuple)
        conn_back.commit()
        logging.info("Inserting exchange API to exchange_API successfully.")

        # Delete API in front DB
        '''try:
            conn_front = mysql.connector.connect(host=frontend_host,
                                       database=frontend_database,
                                       user=frontend_user,
                                       password=frontend_password )
            if conn_front.is_connected():
                print("*** Connected to front DB successfully -- after API insertion to backend ***")
            cursor_front = conn_front.cursor(buffered=True)

            sql_delete_front_query  = """DELETE from `exchange_api_staging` where user_id = %s and exchange_name = %s  """
            delete_front_tuple      = (userId, exchange)
            cursor_front.execute(sql_delete_front_query, delete_front_tuple)
            conn_front.commit()
            logging.info("User API delete from frontend successfully.")
        except Error as e:
            logging.error(e)
        finally:
            conn_back.close()
            logging.info("*** Connection to front DB closed - after API insertion to backend ***")
        '''
            
    except Error as e:
        logging.error(e)
    finally:
        conn_back.close()
        logging.info("Connection to backend DB is closed.")

# We can't subscribe until we've connected, so we use a callback handler
# to subscribe when able
def connect_handler(data):
    channel = pusher.subscribe('my-channel')
    channel.bind('my-event', func_encrypt)

print ("*******************************************************")
print ("**************** Crypto KMS Middleware ****************")
print ("*******************************************************")

while True:
    # Scan if any new API key is created...
    time.sleep(3)
    ts = time.time()
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    
    print("Scanning for new API key ...... ", st)
    search_newkey()
    
    time.sleep(3)
    ts = time.time()
    st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    print("Scanning for key to delete ......", st)
    search_deletekey()


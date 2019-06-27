#!/usr/bin/python
  
###########################################################################
#
# name          : logger.py
#
# purpose       : log errors and other meaningful information
#
# ussage        : import logger
#
# description   :
#
###########################################################################

import datetime
import os

HOME_PATH = os.path.dirname( os.path.realpath( __file__ ) )
if not os.path.exists( os.path.join( HOME_PATH, "Debug" ) ) :
    os.makedirs( os.path.join( HOME_PATH, "Debug" ) )
DEFAULT_DEBUG_PATH = os.path.join( HOME_PATH, "Debug/error.txt" )


###########################################################################
# log_error: error reporting
###########################################################################

def log_error( e, debug_filename=DEFAULT_DEBUG_PATH ) :
    print ("--> Error: %s" % str( e ))
    try :
        with open( debug_filename, 'a' ) as f :
            f.write( "%s - %s\n" % ( datetime.datetime.now().strftime( "%Y-%m-%d %H:%M:%S" ), str( e ) ) )
    except Exception as e :
        print ("--> Error: cannot open debug file: '%s'" % debug_filename)
        print (e)

if __name__ == "__main__" :

    log_error( "This is a test of logger" )

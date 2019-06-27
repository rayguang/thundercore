import time
from datetime import datetime
import pprint
import math
import heapq
import logger
from operator import itemgetter
import random

# Bitfinex v1 and v2 APIs
from bitfinex import ClientV1 as Client1
from bitfinex import ClientV2 as Client2
from bitf_pairs import *

# Init Client
apiKey = 'bqQeZwiyywxyrrYfFB8I8kDyPiSXLAyzo76TQhuHJdq'
apiSecret = 'wuQUWuAaBFO6iqNeLwQZR7Gv5bN8vyLToriBAS3dCTV'
BTFXCLIENT = Client2('apiKey', 'apiSecret')

# Init Calc


def data_log_to_file(message):
    filename = datetime.now().strftime("%Y%m%d")
    with open('results_'+filename+'.txt', 'a+') as f:
        f.write(message)


def get_local_time():
    return datetime.now()


def get_exchange_status():
    # Check exchange status
    exchange_status = BTFXCLIENT.platform_status()             # Paltform Status
    if 1 in exchange_status:
        print("Exchange is operative.\n")
    elif 0 in exchange_status == 0:
        print("Exchange is in maintenance!\n")
    else:
        print("Exchange status unknown {}\n".format(exchange_status))
        exit()


def calc_triarb_profit(list_symbols):

    # Transaction fee %
    fee_maker = 0.1
    fee_taker = 0.2

    # Get order book for symbol
    i = 0
    coin_depth = {}
    for symbol in list_symbols:
        currency_pairs = "Ticker: "+str(symbol)

        orderbook_bid = []
        orderbook_ask = []
        # books for tIOTUSD with price precision 0
        depth = BTFXCLIENT.books('t'+symbol)
        orderbook_bid = [order for order in depth if order[2] > 0]
        orderbook_ask = [order for order in depth if order[2] < 0]
        coin_depth[i] = {
            'symbol': currency_pairs,
            'bid': orderbook_bid[-1][0],
            'bidQuant': orderbook_bid[-1][2],
            'ask': orderbook_ask[0][0],
            'askQuant': orderbook_ask[0][2]
        }
        # print("Symbol {} - bid: {} ask: {}".format(
        #     coin_depth[i]['symbol'], coin_depth[i]['bid'], coin_depth[i]['ask']))

        # print("order book bid: {}\n".format(item))
        # print("order book ask: {}\n".format(item))
        i += 1

        # rate limit control, 10-90 requests per minute
        if i == 3:
            time.sleep(random.uniform(2, 5))

    # Calc cross bid
    time_now = get_local_time()
    cross_bid = float(coin_depth[0]['bid'])*float(coin_depth[1]['bid'])
    market_ask = float(coin_depth[2]['ask'])
    market_bid = float(coin_depth[2]['bid'])
    print("pairs: {} cross bid: {}  market ask {}  market bid {}\n".format(
        list_symbols, cross_bid, market_ask, market_bid))
    # Arbitrage if cross bid greater than market ask
    if (cross_bid > market_ask):
        print("Found potential arbitrage opportunity before fees.")
        gross_pnl = (cross_bid - market_ask - 1)*100
        net_pnl = gross_pnl - 3*fee_taker

        if net_pnl > 0:
            print("Symbol {} - bid: {} ask: {}".format(
                coin_depth[i]['symbol'], coin_depth[i]['bid'], coin_depth[i]['ask']))
            print('Gross PnL: {} Net PnL {}\n'.format(gross_pnl, net_pnl))
            msg = "{} pairs {}\n".format(
                time_now, list_symbols) + "Gross PnL: {}   Net PnL: {}\n".format(gross_pnl, net_pnl)
            data_log_to_file(msg)


def initialize_arb():
    # Welcome banner
    bot_start_time = get_local_time()
    welcome_message = "\nBot Start at Local Time: {}\n".format(bot_start_time)
    print(welcome_message+"\n")
    data_log_to_file(welcome_message)

    time.sleep(1)

    try:
        get_exchange_status()
        for pairs in list_coins:
            # Welcome Banner
            print("Calculating {} {}/{}".format(pairs,
                                                list_coins.index(pairs)+1, len(list_coins)))
            calc_triarb_profit(pairs)
    except Exception as e:
        logger.log_error(e)
        print("Failed to init arbitrage scan!")
        raise


def run():
    # while 1:
    try:
        initialize_arb()
        msg_end = "\n *** Scan completed at {} ***\n".format(get_local_time())
        data_log_to_file(msg_end)
    except:
        print("Init failed. Restarting...\n\n")
        # exit()

    # BTFXCLIENT.platform_status()             # Paltform Status
    # BTFXCLIENT.tickers(['tIOTUSD', 'fIOT'])  # list of tickers
    # BTFXCLIENT.ticker('tIOTUSD')             # ticker
    # BTFXCLIENT.trades('fIOT')                # public trades for symbol
    # BTFXCLIENT.trades('tIOTUSD')             # public trades for symbol
    # # books for tIOTUSD with price precision 0
    # BTFXCLIENT.books('tIOTUSD')
    # # books for tIOTUSD with price precision 1
    # BTFXCLIENT.books('tIOTUSD', "P0")


if __name__ == "__main__":
    run()

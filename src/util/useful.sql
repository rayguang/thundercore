-- Create trading symbol in front-end -- Create a new database called 
SELECT * FROM cryptote_db2.trading_instruments
where exchange_id = 92
and quote_price = "USDC"


update cryptote_db2.trading_instruments
set trading = CONCAT(CONCAT(symbol, "-"),quote_price)
where exchange_id = 92
and quote_price = "USDC" 

-- Trade history sql
SELECT * FROM trading.tradehistory_bitfinex
where uid = 436
and exchange = "BITFINEX"
and time between '1554864552' and '1554864552'

SELECT asset, sum(amount*price) as total_cost, sum(amount) as total_amount FROM trading.tradehistory_bitfinex
where uid = 436
and exchange = "BITFINEX"
and time between 2019-04-10 and 2019-04-10
group by asset

SELECT asset, sum(amount*price) / sum(amount) as total_amount FROM trading.tradehistory_bitfinex
where uid = 436
and exchange = "BITFINEX"
and time between "2019-04-10" and "2019-04-10"
group by asset


SELECT * FROM trading.trade_history
where uid = 436
and exchange = "BITFINEX"
and time between "2019-01-01" and "2019-01-23"


SELECT asset, sum(amount) as total_amount FROM trading.tradehistory_bitfinex
where uid = 436
and exchange = "BITFINEX"
and time between '2019-04-10' and '2019-06-10'
group by asset

SELECT sum(amount), sum(amount*price) FROM trading.tradehistory_bitfinex
where uid = 436
and exchange = "BITFINEX"
and id < 140
order by time desc

-- trade balance SQL
SELECT * FROM trading.account_balance
where exchange = "BINANCE"

select distinct (exchange) from trading.account_balance


SELECT * FROM trading.account_balance;

-- aggregate by assets
select asset, sum(amount) as amount from trading.account_balance
where amount <> 0
and uid = 436
group by asset


-- balance aggregate by exchange 
select exchange, asset, amount from trading.account_balance
where amount <> 0
and uid = 436


select bal.uid, bal.exchange, bal.asset, bal.amount, dh.date from trading.account_balance as bal
join trading.deposit_history dh
where bal.asset = dh.asset
and bal.exchange = dh.exchange

-- Populate portfolio
select bal.uid, bal.exchange, bal.asset, bal.amount, dh.date, ha.close from trading.account_balance as bal
join trading.deposit_history dh
join trading.historyprice_asset ha
where bal.asset = dh.asset
and dh.date = ha.date
and dh.asset = ha.asset
and bal.exchange = dh.exchange



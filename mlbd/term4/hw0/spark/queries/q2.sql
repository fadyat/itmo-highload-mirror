select account_type, count(*) as account_count
from delta.`/opt/spark/delta/accounts`
group by account_type;

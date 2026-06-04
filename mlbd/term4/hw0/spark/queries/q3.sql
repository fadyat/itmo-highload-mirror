select c.full_name, avg(a.balance) AS avg_balance
from delta.`/opt/spark/delta/customers` c
         join delta.`/opt/spark/delta/accounts` a on c.id = a.customer_id
group by c.id, c.full_name
having avg(a.balance) > 100000;

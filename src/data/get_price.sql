select s.id,s.quantity - IFNULL(sum(o.quantity),0) as rest from stock as s
left JOIN order_items as o on s.id = o.stock_id group by s.id
having sum(o.quantity) < sum(s.quantity) or o.id is null
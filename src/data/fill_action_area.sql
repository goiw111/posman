select s.id,p.full_name,s.quantity - IFNULL(sum(o.quantity),0) as rest from stock as s
inner join product as p on s.prod_id = p.id
left JOIN order_items as o on s.id = o.stock_id group by s.id
having sum(o.quantity) < sum(s.quantity) or o.id is null
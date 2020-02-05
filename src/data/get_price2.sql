WITH RECURSIVE domain_family(id,parent_domain_id) AS ( 
select id,parent_domain_id from domains where id is 4 UNION ALL 
select d.id,d.parent_domain_id from domains as d,domain_family as f
where d.id = f.parent_domain_id)
select p.value as price,DATE(s.stock_date)||'('|| ifnull((s.quantity - sum(o.quantity)),s.quantity)||')' as stock_date,
p.stock_id,pr.full_name  as prodect_name,pr.unit_in_pack,pr.inc_unit,pr.property
,s.buy_price,ifnull((s.quantity - sum(o.quantity)),s.quantity) as rest from domains as d1
left join domains as d2 on d2.parent_domain_id = d1.id
inner join prices as p on p.domain_id = d1.id
inner join product as pr on pr.id = s.prod_id and pr.id is 1
inner join stock as s on s.id = p.stock_id
left JOIN order_items as o on s.id = o.stock_id
where domain_id in (SELECT id FROM domain_family) 
and (d2.id is null or d1.parent_domain_id is null) group by s.id
having max(ifnull(d1.parent_domain_id,0.5))
and (sum(o.quantity) < s.quantity or o.id is null)
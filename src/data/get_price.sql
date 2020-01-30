WITH RECURSIVE domain_family(id,parent_domain_id) AS ( 
select id,parent_domain_id from domains where id is 4 UNION ALL 
select d.id,d.parent_domain_id from domains as d,domain_family as f
where d.id = f.parent_domain_id)
select */*s.id,s.stock_date,s.buy_price,p.full_name,p.unit_in_pack,p.inc_unit,p.property,pr.value*/
from stock as s
left JOIN order_items as o on s.id = o.stock_id
inner join product as p on p.id = s.prod_id /*and s.prod_id is 1*/
inner join prices as pr on pr.stock_id = s.id and pr.domain_id = (
SELECT id FROM domain_family)
/*group by s.id
having sum(o.quantity) < sum(s.quantity) or o.id is null*/
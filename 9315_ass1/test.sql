create table mySets (id integer primary key, iset intSet);
insert into mySets values (11, '{1,2,3}');
insert into mySets values (13, '{3, 5, 10, 21, 36, 60, 80, 120, 180, 264, 252, 360, 300, 960, 900, 720, 1080, 1440, 1800, 1680, 2160, 2880, 5616, 3780, 2520, 3600, 6120, 6720, 6300, 5040, 11340, 7560, 14112, 10800, 9240, 10080, 13860, 12600, 31200, 15120, 22680, 20160, 18480, 39312, 33264, 39600, 25200, 30240}');
insert into mySets values (1, '{1,2,3,4,5}');
insert into mySets values (2, '{1,2,3,4,5}');
--insert into mySets values (5, '{  777,  2, 3, 8,1, 0, 7, 8, 9, 5}');
--insert into mySets values (8, '{99,3,8,1,0,7,8,9,5,99,100,101,102,103,104,666,888,999}');
--insert into mySets values (6, '{99,3,8,1,0,7,8,9,5,99,100,101,102,103,104,666,888,999}');
--select * from mySets order by id;
--select id,iset,(# iset) from mySets;
--select a.iset-b.iset ,a.iset from mySets a, mySets b
--where a.id=5 and b.id=8;
--select 3 ? a.iset from mySets a 
--where a.id=4;

--select a.*, b.* from mySets a, mySets b
--where (b.iset <> a.iset) and a.id != b.id;

--select * from mySets where iset @< '{1,2,3,4,5,6}';

select a.iset<>b.iset ,a.iset from mySets a, mySets b
where a.id=1 and b.id=2;
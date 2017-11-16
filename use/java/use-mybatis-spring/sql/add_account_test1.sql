start transaction;
insert into account_info (name) value ('test1');
insert into account_passwd (id, passwd)
value ((select LAST_INSERT_ID()), '123');
commit;

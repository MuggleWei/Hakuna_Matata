# 使用 mysqlclient

## 构建
```
hpb build -c build.yml
```

## 例子
部分例子编自: https://zetcode.com/db/mysqlc/  
* example01_hello: 获取 MYSQL client 版本
* example02_create_db: 创建 database
* example03_create_table: 使用 example02 中创建的 database, 创建表并插入数据
* example04_retrieve_data: 查询 example03 中插入的数据
* example05_inserted_row_id: 插入数据，并获取返回的自增 id 值
* example06_col_headers: 查询 example03 中插入的数据，并且获取列名
* example07_options: 以 ssl 和 charset 为例, 设置 option
* example08_mul_thread: 多线程同时查询 example03 中插入的数据
* example09_stmt_prepare: 使用 prepared statement 插入数据
* example10_dao_mapper: 使用 prepared statement 插入并查询数据

## 一些注意事项

### mysql_store_result vs mysql_use_result
根据 mysql 8.0 的文档: [mysql_user_result()](https://dev.mysql.com/doc/c-api/8.0/en/mysql-use-result.html)，当调用 mysql_real_query() 或 mysql_query() 之后，用户必须为每个成功生成结果集的语句 (SELECT, SHOW, DESCRIBE, EXPLAIN, CHECK TABLE, ...) 调用 mysql_store_result() 或 mysql_use_result()  
mysql_store_reult 和 mysql_use_result 都用于返回结果集，但是它们之间有很大的差异。  
* mysql_store_result
  * 会在调用它时立即检索所有行
  * 从服务器获取行后，为它们分配内存并存储在客户端中
  * 对 mysql_fetch_row 的后续调用保证不会有错误，因为此时只是简单的从已经包含结果集的数据结构中获取一行，mysql_fetch_row 返回 NULL 时就意味着到达了结果集的末尾
* mysql_use_result
  * 启动检索但实际上并未获取任何行
  * 本身不检索任何行，它假定用户稍后将调用 mysql_fetch_row 来检索记录
  * 对 mysql_fetch_row 的调用返回 NULL 时，有可能表示已经到达结果集的末尾，也有可能表示与服务器的通讯发生了错误，需要通过 mysql_errno 或 mysql_error 来区分情况

mysql_store_result 比 mysql_use_result 具有更高的内存和处理要求，因为整个结果集都在客户端维护，当检索大型结果集时，客户端可能面临内存不足的风险。  
mysql_use_result 对内存要求较低，因为一次只需分配足够处理一行的空间即可。但是要注意，mysql_use_result 给服务器带来了更大的负担，它必须保留结果集的行，直到客户端把行取走。如果此时客户端处理很慢将会面临一些问题，因为此时检索数据的表在查询期间处于读取锁定(read-locked)状态，任何试图向这些表进行插入或更新的操作都会被阻止。  
当使用 mysql_use_result 时，客户只能按照检索顺序访问行; 而 mysql_store_result 允许随机访问，比如使用 mysql_data_seek、mysql_row_seek 和 mysql_row_tell。  
使用 mysql_use_result 时，可以获取某些类型的列数据，而使用 mysql_use_result 时却不行。比如结果集中的行数 mysql_num_rows，每列中值的最大宽度存储在 MYSQL_FIELD 列信息结构的 max_width 中  
因为 mysql_use_result 并不像 mysql_store_result 一样将数据全部取回，所以它要求客户端必须为结果集中的每一行调用 mysql_fetch_row，否则，结果集中任何剩余记录都会称为下一个查询结果集的一部分，导致出现 "out of sync" 的错误。  

# 使用 mysqlclient

## 构建
```
# first time
./build_deps.sh

# build examples
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
* example07_options: 设置 option，以 plugin-dir, ssl 和 charset 为例
* example08_mul_thread: 多线程同时查询 example03 中插入的数据
* example09_stmt_prepare: 使用 prepared statement 插入数据
* example10_dao_mapper: 使用 prepared statement 插入并查询数据

## 一些注意事项

### 连接 MySQL 8
根据文档: https://mariadb.com/kb/en/authentication-plugin-sha-256/  
> MySQL 5.6 使用 sha256_password 验证插件，而 MySQL 8.0 则使用 caching_sha2_password 验证插件  

当连接的数据库是 MySQL 而不是 MariaDB 的时候，需要指定插件的位置 (--plugin-dir)，所以当连接 MySQL 时，上面的例子中只有 example07_options 可以通过指定 plugin-dir 后正常使用  

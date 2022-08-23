# sqlite3例子

## 打开数据库
[open_db.c](./open_db/open_db.c) 用于打开(若不存在, 则创建)一个名为hello.db的database

## 创建表
[create_table.c](./create_table/create_table.c) 打开hello.db, 并创建一张名为user的表

## 插入数据
[insert.c](./insert/insert.c) 在上面创建的表中插入数据

## 查询数据
[query.c](./query/query.c) 从user表中读出所有数据

## 更新数据
[update.c](./update/update.c) 将user表中每个人的年龄加1

## 删除数据
[delete.c](./delete/delete.c) 将user表中age小于35的条目删除

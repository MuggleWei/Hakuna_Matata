# docker单点mysql

* 运行`run.sh`拉起mysql
* 运行`stop.sh`停止mysql

## 注意事项
* 在`docker-compose.yml`中, 映射了`/docker-entrypoint-initdb.d`, 将在初始化时执行`./init`文件夹中的所有sql文件
* 在`docker-compose.yml`中, 设置了`MYSQL_ALLOW_EMPTY_PASSWORD=true`, root账户默认是空密码. 又在`./init/init_user.sql`当中, 禁止了root用户的所有远程登录. 这样实现了root账户只有在容器内可以登录, 并且无需密码
* 如果想要给root用户设置密码, 也可以通过`docker-compose.yml`当中配置变量`MYSQL_ROOT_PASSWORD`, 或者设置生成随机密码`MYSQL_RANDOM_ROOT_PASSWORD`来实现

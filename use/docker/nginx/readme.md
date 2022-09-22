# nginx代理

## 脚本
* `gen_ca.sh`: 产生根证书及nginx使用的自签名证书
* `clean_ca.sh`: 清理上面产生的证书
* `run.sh`: 启动整个例子
* `stop.sh`: 停止并清理

## 运行并测试
* 运行`run.sh`将启动例子
* 运行`stop.sh`结束例子并清理文件夹

## 注意事项
* 在`conf.d/nginx.conf`当中, 默认只启动了https, 若想要使用http访问, 打开`listen 80;`即可
* hello-service是一个简单的可以用来测试的小服务
* 把配置文件分开，nginx.conf里，include conf.d，是为了让docker映射文件夹，这样reload可以实时生效
* 当前在`conf.d/nginx.conf`当中, upstream使用了docker-compose默认分配的网络, 并且在`docker-compose.yml`当中关闭了`hello-service`服务的端口映射. 此时, 外部只能通过nginx暴露的端口访问容器内的服务. 若是分布在不同机器上时, 需要配置真实的网络地址

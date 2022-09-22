# nginx代理

## 脚本
* `gen_ca.sh`: 产生根证书及nginx使用的自签名证书
* `clean_ca.sh`: 清理上面产生的证书
* `run.sh`: 启动整个例子
* `stop.sh`: 停止并清理

## 运行并测试
* 运行`run.sh`将启动例子, 注意启动前将`conf.d/nginx.conf`当中的backend的ip地址配置正确
* 运行`stop.sh`结束例子并清理文件夹

## 注意事项
* 在`conf.d/nginx.conf`当中, 默认只启动了https, 若想要使用http访问, 打开`listen 80;`即可
* hello-service是一个简单的可以用来测试的小服务
* 把配置文件分开，nginx.conf里，include conf.d，是为了让docker映射文件夹，这样reload可以实时生效
* 在配置upstream的时候，要写具体的ip地址，别写127.0.0.1，因为运行的时候，那指向了docker容器内部

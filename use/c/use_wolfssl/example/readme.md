# 例子

## echo_serv
一个TCP TLS的Echo服务 [echo_serv.c](./echo_serv/echo_serv.c)

## timer_client
一个TCP TLS的定时发送时间字符串的客户端, 可与echo_serv配合使用, 查看结果 [timer_client.c](./timer_client/timer_client.c)

## passwd_hash
一个典型的密码hash+盐的例子 [passwd_hash.c](./passwd_hash/passwd_hash.c)

## hmac
一个典型的HMAC的例子。例子中先为用户生成AK/SK，接着使用`HMAC(SK, 消息+时间戳+AK)`来生成Hash值 [hmac.c](./hmac/hmac.c)

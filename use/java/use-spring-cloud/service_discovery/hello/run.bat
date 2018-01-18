cd target
start java -jar hello-0.0.1-SNAPSHOT.jar --hello.bind.address=192.168.0.106 --hello.bind.port=8001 --hello.consul.address=192.168.0.151 --hello.consul.port=8500 --hello.version=v0.0.1 --hello.id=1
start java -jar hello-0.0.1-SNAPSHOT.jar --hello.bind.address=192.168.0.106 --hello.bind.port=8002 --hello.consul.address=192.168.0.151 --hello.consul.port=8500 --hello.version=v0.0.1 --hello.id=2
start java -jar hello-0.0.1-SNAPSHOT.jar --hello.bind.address=192.168.0.106 --hello.bind.port=8003 --hello.consul.address=192.168.0.151 --hello.consul.port=8500 --hello.version=v0.0.1 --hello.id=3
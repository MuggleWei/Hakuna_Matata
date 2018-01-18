cd target
start java -jar gate-0.0.1-SNAPSHOT.jar --gate.bind.address=192.168.0.106 --gate.bind.port=9001 --gate.consul.address=192.168.0.151 --gate.consul.port=8500 --gate.version=v0.0.1 --gate.id=1
start java -jar gate-0.0.1-SNAPSHOT.jar --gate.bind.address=192.168.0.106 --gate.bind.port=9002 --gate.consul.address=192.168.0.152 --gate.consul.port=8500 --gate.version=v0.0.1 --gate.id=2
start java -jar gate-0.0.1-SNAPSHOT.jar --gate.bind.address=192.168.0.106 --gate.bind.port=9003 --gate.consul.address=192.168.0.153 --gate.consul.port=8500 --gate.version=v0.0.1 --gate.id=3
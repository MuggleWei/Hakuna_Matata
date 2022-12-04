# 使用 mugglec package

## native
运行 build.sh，将执行自动拉取 mugglec，编译并生成 pkg。随后构建 hello，寻找并链接 mugglec

## docker
先保证本地已经构建了对应的 mugglec 基础镜像(运行 mugglec 工程当中的 build_docker.sh 构建)，随后运行本目录中的 run_docker_hello.sh，将多阶段构建 hello，并最终运行，在 docker_volume/log/ 目录中生成日志文件，并随后删除 docker 容器。

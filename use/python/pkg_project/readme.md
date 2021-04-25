# python打包以及生成可执行文件
此目录展示如何打包python工程, 包括打包模块(package python project), 已经生成可执行文件(pyinstaller)
```
--
 |
 |---mugglefoo (被打包给mugglebar使用)
 |
 |---mugglebar (使用mugglefoo, 并被pyinstaller打包)
```

## package python project
将mugglefoo打包
```
# 进入目录并生成venv
cd mugglefoo
python3 -m venv venv
source venv/bin/activate

# 安装build
pip install build -U

# 打包
python -m build

# 查看打包后到结果, 其中应该有tar.gz(egg包)和whl(wheel包)
cd dist
```

## 使用本地私有包并生成可执行文件
由于这里不想涉及发布的内容, 只展示从本地引用包
```
# 将之前生成到包拷贝到固定目录
mkdir -p ~/pk_pkg
cp mugglefoo/dist/* ~/py_pkg/

# 在mugglebar中使用
cd mugglebar
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
export PYTHONPATH=$PWD
python bar.py

# 生成可执行文件
pyinstaller -F bar.py
```

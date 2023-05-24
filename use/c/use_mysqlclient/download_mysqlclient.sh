#!/bin/bash

# directories
origin_dir="$(dirname "$(readlink -f "$0")")"
build_dir=$origin_dir/build
deps_dir=$origin_dir/_deps
download_dir=$deps_dir/downloads

tmp_mysql_dir=$deps_dir/mysql-tmp
tmp_mysqlclient_dir=$deps_dir/mysqlclient-tmp
mysqlclient_dir=$deps_dir/mysqlclient

if [ ! -d $deps_dir ]; then
	mkdir -p $deps_dir
fi

# prepare mysqlclient
mysql_pkg_name="mysql-8.0.33-linux-glibc2.28-x86_64"
mysql_pkg="$mysql_pkg_name.tar.gz"
mysql_url="https://dev.mysql.com/get/Downloads/MySQL-8.0/$mysql_pkg"

echo "download $mysql_pkg"
if [ ! -f $download_dir/$mysql_pkg ]; then
	wget $mysql_url -P $download_dir/
	status=$?
	if [ $status -eq 0 ]; then
		echo "Success download mysql"
	else
		echo "Failed download mysql"
		exit 1
	fi
else
	echo "$mysql_pkg already exists"
fi

if [ ! -d $mysqlclient_dir ]; then
	if [ -d $tmp_mysql_dir ]; then
		rm -rf $tmp_mysql_dir
	fi
	mkdir -p $tmp_mysql_dir
	tar -xzvf $download_dir/$mysql_pkg -C $tmp_mysql_dir
	mkdir -p $tmp_mysqlclient_dir
	mkdir -p $tmp_mysqlclient_dir/lib
	mkdir -p $tmp_mysqlclient_dir/lib/private
	mv $tmp_mysql_dir/$mysql_pkg_name/include $tmp_mysqlclient_dir/include
	mv $tmp_mysql_dir/$mysql_pkg_name/lib/libmysqlclient* $tmp_mysqlclient_dir/lib/
	mv $tmp_mysql_dir/$mysql_pkg_name/lib/private/libcrypto* $tmp_mysqlclient_dir/lib/private
	mv $tmp_mysql_dir/$mysql_pkg_name/lib/private/libssl* $tmp_mysqlclient_dir/lib/private
	rm -rf $tmp_mysql_dir
	mv $tmp_mysqlclient_dir $mysqlclient_dir
else
	echo "$mysqlclient_dir already exists"
fi

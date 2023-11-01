@echo off

setlocal ENABLEDELAYEDEXPANSION

set BUILD_TYPE=Release

set origin_dir=%~dp0

: build foo 1.1
set foo_1_1_dir=%origin_dir%foo_1.1
cd %foo_1_1_dir%

echo clean %foo_1_1_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)

: build foo 1.0
set foo_1_0_dir=%origin_dir%foo_1.0
cd %foo_1_0_dir%

echo clean %foo_1_0_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)

: build bar 1.0
set bar_1_0_dir=%origin_dir%bar_1.0
cd %bar_1_0_dir%

echo clean %bar_1_0_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)

: build foo 1.2
set foo_1_2_dir=%origin_dir%foo_1.2
cd %foo_1_2_dir%

echo clean %foo_1_2_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)

: build bob 1.0
set bob_1_0_dir=%origin_dir%bob_1.0
cd %bob_1_0_dir%

echo clean %bob_1_0_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)

: build baz 1.0
set baz_1_0_dir=%origin_dir%baz_1.0
cd %baz_1_0_dir%

echo clean %baz_1_0_dir%
if exist build (
	rmdir /s /q build
)
if exist dist (
	rmdir /s /q dist
)
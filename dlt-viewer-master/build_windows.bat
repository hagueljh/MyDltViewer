@echo off

set QTDIR=D:\Qt\Qt5.5.1\5.5\mingw492_32
set MINGW_DIR=D:\Qt\Qt5.5.1\Tools\mingw492_32

START /wait build_parser_windows_qt5.bat
START /wait build_sdk_windows_qt5.bat

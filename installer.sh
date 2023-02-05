#!/bin/bash

# インストールの場所取得
resource_path=$(dirname $0)

# 絶対パスか相対パスか判定
if [ ${resource_path} != "/*" ]; then
    dirname="$(cd -- "$(dirname -- "$resource_path")" && pwd)" || exit $?
    resource_path="${dirname%/}/$(basename -- "$resource_path")"
    resource_abs_path_list=(${resource_path//// })
    resource_path="/"
    for rpal in ${resource_abs_path_list[@]}; do
        if [ "$rpal" != "." ]; then
            if [ "$rpal" != ".." ]; then
                resource_path+="$rpal/"
            fi
        fi
    done
else
    if [ ${resource_path} != "*/" ];then
        resource_path+="/"
    fi
fi

# git submoduleをクローンしておく
git submodule update --init --recursive

# configファイル書き込み
PYTHON_VERSION=($(python3 -V))
echo -e "\ndefault_python_version = ${PYTHON_VERSION[1]}" >> src/config/env.cfg

# ビルドディレクトリ作成し移動
mkdir build
cd build

# インストール先作成
INSTALL_SPACE=$HOME/venv_space
if [ ! -d $INSTALL_SPACE ]; then
    mkdir $INSTALL_SPACE
fi

# ビルド
JOBS=$($(grep cpu.cores /proc/cpuinfo | sort -u | sed 's/[^0-9]//g') + 1)
cmake ../src/ -DCMAKE_INSTALL_PREFIX=$INSTALL_SPACE
make -j$JOBS
make install

# 補完機能のスクリプト作成
cat scripts/complementHead.sh >> $INSTALL_SPACE/bin/complement.sh
echo -e "" >> $INSTALL_SPACE/bin/complement.sh
cat scripts/complementMain.sh >> $INSTALL_SPACE/bin/complement.sh

# Pythonインストール
$INSTALL_SPACE/bin/venv_tool install ${PYTHON_VERSION[1]}

# bashrcに書き込み
echo -e "\n# venvの設定" >> $HOME/.bashrc
echo -e ". $INSTALL_SPACE/bin/complement.sh\n" >> $HOME/.bashrc
#!/bin/bash

function _argAnalyzer() {
    while getopts :i:p:u-: OPT; do
        local long_option_flag=false

        if [ "$OPT" == "-" ]; then
            if [[ "$OPTARG" == -* ]]; then
                echo "引数が不正です"
                exit 1
            fi
            key="--$OPTARG"
            value="${!OPTIND}"
            long_option_flag=true
        else
            key="-$OPT"
            value="$OPTARG"
        fi

        case "$key" in
        -p | --python)
            echo "Python version が指定された"
            PYTHON_VERSION=$value
            if "${long_option_flag}"; then
                shift
            fi
            ;;
        -i | --install_space)
            echo "インストール先 が指定された"
            local path=$value
            if [[ "$value" == */ ]]; then
                path=${value/%?/}
            fi
            INSTALL_SPACE=$path
            if "${long_option_flag}"; then
                shift
            fi
            ;;
        -u | --update)
            echo "上書き が指定された"
            NO_UPDATE_FLAG=false
            ;;
        *)
            echo "その引数$keyはリストにありません"
            exit 1
            ;;
        esac
    done
}

# インストールファイルを実行した場所の取得
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
    if [ ${resource_path} != "*/" ]; then
        resource_path+="/"
    fi
fi

# 変数の一覧(規定値)
PYTHON_VERSION=($(python3 -V))
INSTALL_SPACE=$HOME/.venv
NO_UPDATE_FLAG=true

# 引数分析
_argAnalyzer $@

# git submoduleをクローンしておく
git submodule update --init --recursive --depth 1

# configファイル書き込み
echo -e "\"default_python_version\" : \"${PYTHON_VERSION[1]}\"\n}" >>src/config/env.json

# ビルドディレクトリ作成し移動
if [ -d build ]; then
    rm -rf build
fi
mkdir build
cd build

# インストール先作成
if [ ! -d $INSTALL_SPACE ]; then
    mkdir $INSTALL_SPACE
fi

# ビルド
JOBS=$(grep cpu.cores /proc/cpuinfo | sort -u | sed 's/[^0-9]//g')
JOBS=$((JOBS + 1))
cmake ../src/ -DCMAKE_INSTALL_PREFIX=$INSTALL_SPACE
make -j$JOBS
make install

# 補完機能のスクリプト作成
cat scripts/complementHead.sh >>$INSTALL_SPACE/bin/complement.sh
echo -e "" >>$INSTALL_SPACE/bin/complement.sh
cat scripts/complementMain.sh >>$INSTALL_SPACE/bin/complement.sh

# Pythonインストール
$INSTALL_SPACE/bin/venv_tool install ${PYTHON_VERSION[1]}

# bashrcに書き込み
if "${NO_UPDATE_FLAG}"; then
    echo -e "\n# venvの設定" >>$HOME/.bashrc
    echo -e ". $INSTALL_SPACE/bin/complement.sh\n" >>$HOME/.bashrc
fi

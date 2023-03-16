# venv_tool
venvを使いやすくするツール


## インストール方法

1. システムの更新
    ```bash
    sudo apt-get update && sudo apt-get -y upgrade
    ```

2. 関連パッケージのインストール
    ```bash
    sudo apt-get -y install cmake \
    gcc g++ language-pack-ja\
    build-essential git libbz2-dev libdb-dev \
    libffi-dev libgdbm-dev liblzma-dev \
    libncursesw5-dev libreadline-dev libsqlite3-dev \
    libssl-dev tk-dev uuid-dev \
    wget zlib1g-dev
    ```

3. リポジトリのクローン
    ```bash
    git clone https://github.com/shuheiEasy/venv_tool.git
    ```
4. クローンした場所へ移動
    ```bash
    cd venv_tool
    ```
5. インストール
    ```bash
    ./installer.sh
    ```
    インストールする場所を指定する場合
    ```bash
    ./installer.sh -i [インストールしたい場所]
    ```
6. bashrcの読込
    ```bash
    source ~/.bashrc
    ```
7. インストールの確認
    ```bash
    venv version
    ```
    このときにバージョンが表示されればインストール成功となる。


## 主なコマンド

- 仮想環境の作成
    ```bash
    venv create [仮想環境の名前]
    ```
    仮想環境のPythonバージョンを指定する場合
    ```bash
    venv create [仮想環境の名前] [Pythonのバージョン]
    ```
- 仮想環境の起動
    ```bash
    venv activate [仮想環境の名前]
    ```
- 仮想環境の終了
    ```bash
    venv deactivate
    ```
- 仮想環境の一覧
    ```bash
    venv env
    ```
    Pythonのバージョンも確認したい場合
    ```bash
    venv env show
    ```
- 仮想環境の削除
    ```bash
    venv remove [仮想環境の名前]
    ```

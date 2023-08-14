#ifndef VENV_TOOL_CORE_H
#define VENV_TOOL_CORE_H

#include <dataObject/dataObject.hpp>
#include <logSystem/logSystem.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "venv_tool_config.h"

namespace venv_tool
{
    // Pythonのバージョン
    class PythonVersion : public dataObject::DataObject
    {
    private:
        int major;
        int minor;
        int patch;

    public:
        PythonVersion(dataObject::String version);
        ~PythonVersion() {}
        int getMajor() const;
        int getMinor() const;
        int getPatch() const;
        dataObject::String getVersion() const;
        bool operator>(const PythonVersion &data) const;
    };

    // 実行環境の状態クラス
    class EnvState : public dataObject::DataObject
    {
    private:
        // パス関連
        dataObject::String venv_path;
        dataObject::String venv_virtual_env_path;

        // 状態変数
        enum State
        {
            REAL,
            VIRTUAL
        };
        State state;
        dataObject::String virtual_env_name;

        // 仮想環境のリスト
        dataObject::List<dataObject::String> env_name_list;
        dataObject::List<dataObject::String> env_path_list;

        // 関数群
        void _getEnvList();
        void _getEnvState();

    public:
        EnvState(const dataObject::String &venvPath);
        ~EnvState();
        dataObject::List<dataObject::String> getEnvNameList() const;
        dataObject::List<dataObject::String> getEnvPathList() const;
        bool getEnvState(dataObject::String &env_name);
        void update();
    };

    // 仮想環境の起動
    bool activateEnv(dataObject::String env_name, dataObject::String &venv_path, EnvState &state);

    // 設定項目の追加
    void addConfig(dataObject::List<dataObject::String> args, fileSystem::JsonFile &config_file);

    // パスの追加
    void addPath(dataObject::String path, dataObject::String &venv_path, EnvState &state);

    // Pip設定の追加
    void addPipConfig(dataObject::List<dataObject::String> args, dataObject::Dict<dataObject::String, dataObject::Dict<dataObject::String, dataObject::List<dataObject::String>>> &pip_cfgs);

    // 実行環境の確認
    // 状況に応じて初期化する
    void checkEnv(fileSystem::JsonFile &cfg_json_file);

    // 外部コマンド実行
    dataObject::List<dataObject::String> command(dataObject::String cmd);

    // 仮想環境の作成
    int createEnv(dataObject::String env_name, dataObject::String venv_path, PythonVersion python_version);

    // Help文出力
    void help_text(void);

    // 仮想環境へパスを通す
    void setEnvironmentPath(dataObject::String env_name, dataObject::String append_dir_path, dataObject::Dict<dataObject::String, dataObject::String> &cfg);

    // 仮想環境へ通っているパスの一覧を取得
    dataObject::List<dataObject::String> pathList(dataObject::String &venv_path);

    // リストを標準出力する
    void printList(dataObject::List<dataObject::String> list);

    // Pythonのインストール
    int pythonInstall(dataObject::String venv_path, PythonVersion version);

    // インストールしたPythonのバージョンを取得
    dataObject::List<dataObject::String> pythonList(dataObject::String &venv_path);

    // pip設定ファイル読込
    void readPipConfig(dataObject::String pip_cfg_path, dataObject::Dict<dataObject::String, dataObject::Dict<dataObject::String, dataObject::List<dataObject::String>>> &cfg);

    // 仮想環境の削除のUI
    int removeEnvConfirm(dataObject::String env_name, dataObject::String &venv_path, EnvState &state);

    // 仮想環境の削除の実行部分
    void removeEnvExecute(dataObject::String env_name, dataObject::String &venv_path);

    // パスの削除
    void removePath(dataObject::String path, dataObject::String &venv_path, EnvState &state);

    // Pip設定の削除
    void removePipConfig(dataObject::List<dataObject::String> args, dataObject::Dict<dataObject::String, dataObject::Dict<dataObject::String, dataObject::List<dataObject::String>>> &pip_cfgs);

    // pip設定ファイル書込
    void writePipConfig(dataObject::String pip_cfg_path, dataObject::Dict<dataObject::String, dataObject::Dict<dataObject::String, dataObject::List<dataObject::String>>> pip_cfgs);
}

#endif
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
        EnvState(dataObject::Dict<dataObject::String, dataObject::String> cfg);
        ~EnvState();
        dataObject::List<dataObject::String> getEnvNameList() const;
        dataObject::List<dataObject::String> getEnvPathList() const;
        bool getEnvState(dataObject::String &env_name);
        void update();
    };

    bool activateEnv(dataObject::String env_name, dataObject::Dict<dataObject::String, dataObject::String> &cfg, EnvState &state);
    void checkEnv(dataObject::String env_cfg_path, dataObject::Dict<dataObject::String, dataObject::String> &cfg);
    dataObject::List<dataObject::String> command(dataObject::String cmd);
    int createEnv(dataObject::String env_name, dataObject::String venv_path, PythonVersion python_version);
    dataObject::String getCurrentDirectory(void);
    void help_text(void);
    void moveCurrentDirectory(dataObject::String path);
    void setEnvironmentPath(dataObject::String env_name, dataObject::String append_dir_path, dataObject::Dict<dataObject::String, dataObject::String> &cfg);
    void printList(dataObject::List<dataObject::String> list);
    int pythonInstall(dataObject::String venv_path, PythonVersion version);
    dataObject::List<dataObject::String> pythonList(dataObject::Dict<dataObject::String, dataObject::String> &cfg);
    void readConfig(dataObject::String cfg_dir_path, dataObject::Dict<dataObject::String, dataObject::String> &cfg);
    void writeConfig(dataObject::String cfg_dir_path, dataObject::Dict<dataObject::String, dataObject::String> cfg);
}

#endif
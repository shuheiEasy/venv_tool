#include "venv_tool.hpp"

namespace venv_tool
{
    using namespace dataObject;
    using namespace fileSystem;
    using namespace logSystem;

    EnvState::EnvState(Dict<String, String> cfg)
    {
        venv_path = cfg["venv_path"];
        venv_virtual_env_path = venv_path + "env";
        update();
    }

    EnvState::~EnvState() {}

    List<String> EnvState::getEnvNameList() const
    {
        return this->env_name_list;
    }

    List<String> EnvState::getEnvPathList() const
    {
        return this->env_path_list;
    }
    bool EnvState::getEnvState(String &env_name)
    {
        if(this->state==REAL){
            env_name = "";
            return false;
        }else{
            env_name = this->virtual_env_name;
            return true;
        }
    }

    void EnvState::update()
    {
        _getEnvList();
        _getEnvState();
    }

    void EnvState::_getEnvList()
    {
        FileExplorer fe(venv_virtual_env_path);
        auto env_dir_list = fe.getDirList();

        env_name_list.clear();
        env_path_list.clear();
        for (int i = 0; i < env_dir_list.getSize(); i++)
        {
            env_name_list.append(env_dir_list[i].getName());
            env_path_list.append(env_dir_list[i].getPath());
        }
    }

    void EnvState::_getEnvState()
    {
        // 環境パス取得
        auto envPathList = venv_virtual_env_path.split("/");

        // Pythonパス取得
        List<String> rets = command("which python3");
        String pyPath = rets[0];
        auto pyPathList = pyPath.split("/");

        // 判定
        Bool checkFlag = true;
        if (len(envPathList) < len(pyPathList))
        {
            for (int i = 0; i < len(envPathList); i++)
            {
                if (pyPathList[i] != envPathList[i])
                {
                    checkFlag = false;
                    break;
                }
            }
        }
        else
        {
            checkFlag = false;
        }

        if (checkFlag)
        {
            state = VIRTUAL;
            virtual_env_name = pyPathList[len(envPathList)];
        }
        else
        {
            state = REAL;
            virtual_env_name = "";
        }
    }
}
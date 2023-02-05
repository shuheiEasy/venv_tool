#include "venv_tool.hpp"

using namespace dataObject;
using namespace logSystem;
using namespace venv_tool;

int main(int argc, char *argv[])
{
    // 引数のリスト
    List<String> args;
    for (int i = 1; i < argc; i++)
    {
        args.append(argv[i]);
    }

    // 環境変数
    String env_cfg_path(VENV_TOOL_ENV_CONFIG_FILE);
    Dict<String, String> cfgs;
    readConfig(env_cfg_path, cfgs);

    // 環境の確認
    checkEnv(env_cfg_path, cfgs);

    // 状態の取得
    venv_tool::EnvState envState(cfgs);

    // 引数解析
    if (len(args) == 0)
    {
        print("引数が指定されていません");
        help_text();
    }
    else
    {

        if (args[0] == "activate")
        {
            auto flag = activateEnv(args[1], cfgs, envState);
            if (flag)
            {
                return 10;
            }
            else
            {
                return 11;
            }
        }
        else if (args[0] == "create")
        {
            int ret = -1;
            switch (len(args))
            {
            case 1:
                print("バージョン情報が足りません");
                break;
            case 2:
                ret = createEnv(args[1], cfgs["venv_path"], PythonVersion(cfgs["default_python_version"]));
                if (ret == 0)
                {
                    return 30;
                }
                break;
            case 3:
                ret = createEnv(args[1], cfgs["venv_path"], PythonVersion(args[2]));
                if (ret == 0)
                {
                    return 30;
                }
                break;
            default:
                print("引数が多すぎます。create [仮想環境名]のようにしてください");
                break;
            }
        }
        else if (args[0] == "deactivate")
        {
            String buf;
            auto flag = envState.getEnvState(buf);
            if (flag)
            {
                return 20;
            }
        }
        else if (args[0] == "env")
        {
            if (len(args) == 1)
            {
                printList(envState.getEnvNameList());
            }
            else if (len(args) == 2)
            {
                if (args[1] == "name")
                {
                    printList(envState.getEnvNameList());
                }
                else if (args[1] == "path")
                {
                    auto list = envState.getEnvPathList();
                    for (int i = 0; i < len(list); i++)
                    {
                        print(list[i]);
                    }
                }
                else
                {
                    print("envの引数は「name」か「path」です");
                }
            }
            else
            {
                print("引数が多すぎます。");
            }
        }
        else if (args[0] == "install")
        {
            if (len(args) == 1)
            {
                print("バージョン情報が足りません");
            }
            else if (len(args) > 2)
            {
                print("引数が多すぎます。install VERSIONのようにしてください");
            }
            else
            {
                auto ret = pythonInstall(cfgs["venv_path"], PythonVersion(args[1]));
                if (ret < 0)
                {
                    print("すでにインストールされています");
                }
            }
        }
        
        else if (args[0] == "python")
        {
            List<String> pythons = pythonList(cfgs);
            if (len(args) == 1)
            {
                for (int i = 0; i < len(pythons); i++)
                {
                    if (cfgs["default_python_version"] == pythons[i])
                    {
                        print("* ", pythons[i]);
                    }
                    else
                    {
                        print("  ", pythons[i]);
                    }
                }
            }
            else if (len(args) == 2)
            {
                if (args[1] == "list")
                {
                    printList(pythons);
                }
                else
                {
                    print("pythonコマンドのオプションはlistかdefaultのみです。");
                }
            }
            else if (len(args) == 3)
            {
                if (args[1] == "default")
                {
                    // 存在確認
                    bool exist_flag=false;
                    for(int i = 0; i < len(pythons); i++){
                        if(args[2]==pythons[i]){
                            exist_flag=true;
                            break;
                        }
                    }

                    if(exist_flag){
                        cfgs["default_python_version"]=args[2];
                        writeConfig(env_cfg_path,cfgs);
                    }else{
                        print("指定したPython ",args[2],"はインストールされていません");
                    }

                }
                else
                {
                    print("pythonコマンドのオプションはlistかdefaultのみです。");
                }
            }
            else
            {
                print("pythonコマンドのオプションはlistのみです。");
            }
        }
        else if (args[0] == "remove")
        {
            print("remove");
        }
        else if (args[0] == "version")
        {
            print(VENV_TOOL_VERSION);
        }
        else
        {
            print("引数に誤りがあります");
            help_text();
        }
    }

    return 0;
}
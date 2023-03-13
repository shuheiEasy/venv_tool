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

    // 環境変数の読込
    fileSystem::JsonFile env_config(VENV_TOOL_ENV_CONFIG_FILE);
    if (env_config.read() < 0)
    {
        print("JSONファイルが読めません");
        return -1;
    }

    Dict<String, Any> &configs = env_config.getDict();

    // 環境の確認
    checkEnv(env_config);

    // 状態の取得
    venv_tool::EnvState envState(*(configs["venv_path"].getData<String>()));

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
            auto flag = activateEnv(args[1], *(configs["venv_path"].getData<String>()), envState);
            if (flag)
            {
                return 10;
            }
            else
            {
                return 11;
            }
        }
        else if (args[0] == "configure")
        {
            if (len(args) == 1)
            {
                for (auto e : configs)
                {
                    print(e.key, " : ", e.value);
                }
            }
            else
            {
                if (args[1] == "pip")
                {
                    String env_name;

                    if (len(args) == 2)
                    {
                        Dict<String, Dict<String, List<String>>> pip_cfgs;

                        String pip_cfg_path = *(configs["venv_path"].getData<String>());
                        if (envState.getEnvState(env_name))
                        {
                            pip_cfg_path += "env/";
                            pip_cfg_path += env_name;
                        }
                        else
                        {
                            pip_cfg_path += "config";
                        }
                        pip_cfg_path += "/pip.conf";

                        readPipConfig(pip_cfg_path, pip_cfgs);

                        auto orders = pip_cfgs.getKeys();
                        for (int i = 0; i < len(orders); i++)
                        {
                            print(orders[i]);
                            auto keys = pip_cfgs[orders[i]].getKeys();

                            for (int j = 0; j < len(keys); j++)
                            {
                                String outtext;
                                auto buffer_list = pip_cfgs[orders[i]][keys[j]];
                                for (int k = 0; k < len(buffer_list); k++)
                                {
                                    if (k > 0)
                                    {
                                        outtext += ", ";
                                    }
                                    outtext += buffer_list[k];
                                }

                                print(keys[j], " = ", outtext);
                            }
                        }
                    }
                    else
                    {
                        if (len(args) == 3)
                        {
                            print("引数が足りません");
                        }
                        else
                        {
                            if (args[2] == "add")
                            {
                                if (envState.getEnvState(env_name))
                                {
                                    String pip_cfg_path = *(configs["venv_path"].getData<String>()) + "env/" + env_name + "/pip.conf";
                                    Dict<String, Dict<String, List<String>>> pip_cfgs;
                                    readPipConfig(pip_cfg_path, pip_cfgs);
                                    addPipConfig(args, pip_cfgs);
                                    writePipConfig(pip_cfg_path, pip_cfgs);
                                }
                                else
                                {
                                    auto env_path_list = envState.getEnvPathList();
                                    for (int i = 0; i < env_path_list.getSize(); i++)
                                    {
                                        String pip_cfg_path = env_path_list[i] + "pip.conf";
                                        Dict<String, Dict<String, List<String>>> pip_cfgs;
                                        readPipConfig(pip_cfg_path, pip_cfgs);
                                        addPipConfig(args, pip_cfgs);
                                        writePipConfig(pip_cfg_path, pip_cfgs);
                                    }

                                    String pip_cfg_path = *(configs["venv_path"].getData<String>()) + "config/pip.conf";
                                    Dict<String, Dict<String, List<String>>> pip_cfgs;
                                    readPipConfig(pip_cfg_path, pip_cfgs);
                                    addPipConfig(args, pip_cfgs);
                                    writePipConfig(pip_cfg_path, pip_cfgs);
                                }
                            }
                        }
                    }
                }
                else
                {
                    print("この引数は不正です");
                }
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
                ret = createEnv(args[1], *(configs["venv_path"].getData<String>()), PythonVersion(*(configs["default_python_version"].getData<String>())));
                if (ret == 0)
                {
                    return 30;
                }
                break;
            case 3:
                ret = createEnv(args[1], *(configs["venv_path"].getData<String>()), PythonVersion(args[2]));
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
                auto env_name_list = envState.getEnvNameList();
                String active_env_name;
                envState.getEnvState(active_env_name);

                for (int i = 0; i < len(env_name_list); i++)
                {
                    if (env_name_list[i] == active_env_name)
                    {
                        print("* ", env_name_list[i]);
                    }
                    else
                    {
                        print("  ", env_name_list[i]);
                    }
                }
            }
            else if (len(args) == 2)
            {
                if (args[1] == "list")
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
                else if (args[1] == "show")
                {
                    auto env_name_list = envState.getEnvNameList();
                    auto env_path_list = envState.getEnvPathList();
                    String active_env_name;
                    String env_name = "-";
                    String PYTHONVERSIONTEXT = "python version ";
                    int ENV_LEN = 32;
                    envState.getEnvState(active_env_name);
                    for (int i = len(env_name); i < ENV_LEN + len(PYTHONVERSIONTEXT) + 1; i++)
                    {
                        env_name += "-";
                    }
                    print("+---", env_name, "-", "+");
                    env_name = "env_name";
                    for (int i = len(env_name); i < ENV_LEN + 1; i++)
                    {
                        env_name += " ";
                    }
                    print("|   ", env_name, " ", PYTHONVERSIONTEXT, "|");
                    env_name = "=";
                    for (int i = len(env_name); i < ENV_LEN + len(PYTHONVERSIONTEXT) + 1; i++)
                    {
                        env_name += "=";
                    }
                    print("|===", env_name, "=", "|");

                    for (int i = 0; i < len(env_name_list); i++)
                    {
                        String cmd = env_path_list[i];
                        cmd += "/bin/python3 -V";
                        auto ret = command(cmd);
                        auto python_version_name = ret[0].split(" ")[1];
                        for (int i = len(python_version_name); i < len(PYTHONVERSIONTEXT); i++)
                        {
                            python_version_name += " ";
                        }
                        auto env_name = env_name_list[i];
                        for (int i = len(env_name); i < ENV_LEN + 1; i++)
                        {
                            env_name += " ";
                        }

                        if (env_name_list[i] == active_env_name)
                        {
                            print("| * ", env_name, " ", python_version_name, "|");
                        }
                        else
                        {
                            print("|   ", env_name, " ", python_version_name, "|");
                        }
                    }
                    env_name = "-";
                    for (int i = len(env_name); i < ENV_LEN + len(PYTHONVERSIONTEXT) + 1; i++)
                    {
                        env_name += "-";
                    }
                    print("+---", env_name, "-", "+");
                }
                else if (args[1] == "state")
                {
                    String active_env_name;
                    bool virtual_flag = envState.getEnvState(active_env_name);
                    if (virtual_flag)
                    {
                        print("VIRTUAL ", active_env_name);
                    }
                    else
                    {
                        print("REAL");
                    }
                }
                else
                {
                    print("envの引数は「name」か「path」か「show」か「state」です");
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
                auto ret = pythonInstall(*(configs["venv_path"].getData<String>()), PythonVersion(args[1]));
                if (ret < 0)
                {
                    print("すでにインストールされています");
                }
            }
        }
        else if (args[0] == "path")
        {
            if (len(args) == 1)
            {
                auto paths = pathList(*(configs["venv_path"].getData<String>()));
                for (int i = 1; i < len(paths); i++)
                {
                    print(paths[i]);
                }
            }
            else if (len(args) == 2)
            {
                if (args[1] == "add")
                {
                    addPath(".", *(configs["venv_path"].getData<String>()), envState);
                }
                else if (args[1] == "list")
                {
                    auto paths = pathList(*(configs["venv_path"].getData<String>()));
                    paths.del(0);
                    printList(paths);
                }
            }
            else
            {
                if (args[1] == "add")
                {
                    for (int i = 2; i < len(args); i++)
                    {
                        addPath(args[i], *(configs["venv_path"].getData<String>()), envState);
                    }
                }
                else
                {
                    print("引数が多すぎです");
                }
            }
        }
        else if (args[0] == "python")
        {
            List<String> pythons = pythonList(*(configs["venv_path"].getData<String>()));
            if (len(args) == 1)
            {
                String default_python_version = *(configs["default_python_version"].getData<String>());
                for (int i = 0; i < len(pythons); i++)
                {
                    if (default_python_version == pythons[i])
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
                    bool exist_flag = false;
                    for (int i = 0; i < len(pythons); i++)
                    {
                        if (args[2] == pythons[i])
                        {
                            exist_flag = true;
                            break;
                        }
                    }

                    if (exist_flag)
                    {
                        configs["default_python_version"] = args[2];
                        env_config.write();
                    }
                    else
                    {
                        print("指定したPython ", args[2], "はインストールされていません");
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

            if (len(args) == 1)
            {
                print("削除する環境名が必要です");
            }
            else if (len(args) == 2)
            {
                auto flag = removeEnvConfirm(args[1], *(configs["venv_path"].getData<String>()), envState);
                switch (flag)
                {
                case 0:
                    return 40;
                case 1:
                    return 41;
                case 2:
                    return 42;
                case 3:
                    return 43;
                default:
                    break;
                }
            }
            else
            {
                print("引数が多すぎます。remove [仮想環境名]のようにしてください");
            }
        }
        else if (args[0] == "remove_environment")
        {
            removeEnvExecute(args[1], *(configs["venv_path"].getData<String>()));
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
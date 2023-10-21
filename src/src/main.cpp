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
            if (len(args) == 1)
            {
                print("値が指定されていません");
            }

            if (args[1] != "--help")
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

            help_text_activate();
            return -1;
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
                if (args[1] == "--help")
                {
                    help_text_configure();
                }
                else if (args[1] == "add")
                {
                    if (len(args) == 2)
                    {
                        print("値が指定されていません");
                        help_text_configure();
                    }
                    else
                    {
                        addConfig(args.slice(2, args.getSize()), env_config);
                    }
                }
                else if (args[1] == "show")
                {
                    for (auto e : configs)
                    {
                        print(e.key, " : ", e.value);
                    }
                }
                else
                {
                    print("この引数は不正です");
                    help_text_configure();
                }
            }
        }
        else if (args[0] == "create")
        {
            int ret = -1;
            switch (len(args))
            {
            case 1:
                print("値が指定されていません");
                help_text_create();
                break;
            case 2:
                if (args[1] == "--help")
                {
                    break;
                }
                ret = createEnv(args[1], *(configs["venv_path"].getData<String>()), PythonVersion(*(configs["default_python_version"].getData<String>())));
                if (ret == 0)
                {
                    return 30;
                }
                break;
            case 3:
                if (args[1] == "--help")
                {
                    break;
                }
                ret = createEnv(args[1], *(configs["venv_path"].getData<String>()), PythonVersion(args[2]));
                if (ret == 0)
                {
                    return 30;
                }
                break;
            default:
                print("引数が多すぎます.");
                break;
            }

            help_text_create();
            return -1;
        }
        else if (args[0] == "deactivate")
        {
            if (len(args) > 1)
            {
                if (args[1] != "--help")
                {
                    print("この引数は不正です");
                }
            }
            else
            {
                String buf;
                auto flag = envState.getEnvState(buf);
                if (flag)
                {
                    return 20;
                }
            }

            help_text_deactivate();
            return -1;
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
                if (args[1] == "--help")
                {
                    help_text_env();
                }
                else if (args[1] == "list")
                {
                    printList(envState.getEnvNameList());
                }
                else if (args[1] == "path")
                {
                    auto names = envState.getEnvNameList();
                    auto paths = envState.getEnvPathList();

                    if (len(names) != len(paths))
                    {
                        print("予期せぬエラーが発生しました.");
                    }
                    else
                    {
                        for (int i = 0; i < len(paths); i++)
                        {
                            print(names[i], " : ", paths[i]);
                        }
                    }
                }
                else if (args[1] == "show")
                {
                    // 環境取得
                    auto env_name_list = envState.getEnvNameList();
                    auto env_path_list = envState.getEnvPathList();

                    // 定数
                    Dict<String, String> OUTPUTTEXT;
                    OUTPUTTEXT["env_name"] = "name";
                    OUTPUTTEXT["version"] = "version";
                    OUTPUTTEXT["path"] = "path";
                    OUTPUTTEXT["active"] = " * ";
                    OUTPUTTEXT["space"] = " ";
                    OUTPUTTEXT["width"] = "-";
                    OUTPUTTEXT["height"] = "|";
                    OUTPUTTEXT["corner"] = "+";
                    OUTPUTTEXT["border"] = "=";

                    // 実行環境取得
                    String active_env_name;
                    envState.getEnvState(active_env_name);

                    // Pythonのバージョンリスト取得
                    List<String> env_version_list;
                    for (int i = 0; i < len(env_name_list); i++)
                    {
                        String cmd = env_path_list[i];
                        cmd += "/bin/python3 -V";
                        auto ret = command(cmd);
                        auto python_version_name = ret[0].split(" ")[1];
                        env_version_list.append(python_version_name);
                    }

                    // 出力の長さ
                    Dict<String, Int> TEXTLENGTH;
                    TEXTLENGTH["space"] = 1;
                    TEXTLENGTH["flag"] = len(OUTPUTTEXT["active"]);
                    TEXTLENGTH["name"] = len(OUTPUTTEXT["env_name"]) + TEXTLENGTH["space"];
                    TEXTLENGTH["version"] = len(OUTPUTTEXT["version"]) + TEXTLENGTH["space"];
                    TEXTLENGTH["path"] = len(OUTPUTTEXT["path"]) + TEXTLENGTH["space"];

                    for (int i = 0; i < len(env_name_list); i++)
                    {
                        if (TEXTLENGTH["name"] < len(env_name_list[i]) + TEXTLENGTH["space"])
                        {
                            TEXTLENGTH["name"] = len(env_name_list[i]) + TEXTLENGTH["space"];
                        }
                        if (TEXTLENGTH["version"] < len(env_version_list[i]) + TEXTLENGTH["space"])
                        {
                            TEXTLENGTH["version"] = len(env_version_list[i]) + TEXTLENGTH["space"];
                        }
                        if (TEXTLENGTH["path"] < len(env_path_list[i]) + TEXTLENGTH["space"])
                        {
                            TEXTLENGTH["path"] = len(env_path_list[i]) + TEXTLENGTH["space"];
                        }
                    }

                    /////////////////////////////////////////////////////////////
                    // 出力
                    /////////////////////////////////////////////////////////////

                    // 1行目
                    String output = OUTPUTTEXT["corner"];
                    for (int i = 0; i < TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"] + TEXTLENGTH["space"] + TEXTLENGTH["path"]; i++)
                    {
                        output += OUTPUTTEXT["width"];
                    }
                    output += OUTPUTTEXT["corner"];
                    print(output);

                    // 2行目
                    output = OUTPUTTEXT["height"];
                    for (int i = 0; i < TEXTLENGTH["flag"]; i++)
                    {
                        output += OUTPUTTEXT["space"];
                    }
                    output += OUTPUTTEXT["env_name"];
                    while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"])
                    {
                        output += OUTPUTTEXT["space"];
                    }
                    output += OUTPUTTEXT["version"];
                    while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"]+ TEXTLENGTH["space"])
                    {
                        output += OUTPUTTEXT["space"];
                    }
                    output += OUTPUTTEXT["path"];
                    while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"] + TEXTLENGTH["space"] + TEXTLENGTH["path"])
                    {
                        output += OUTPUTTEXT["space"];
                    }
                    output += OUTPUTTEXT["height"];
                    print(output);

                    // 3行目
                    output = OUTPUTTEXT["height"];
                    for (int i = 0; i < TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"]+ TEXTLENGTH["space"] + TEXTLENGTH["path"]; i++)
                    {
                        output += OUTPUTTEXT["border"];
                    }
                    output += OUTPUTTEXT["height"];
                    print(output);

                    // 4行目以降
                    for (int env_id = 0; env_id < len(env_name_list); env_id++)
                    {
                        output = OUTPUTTEXT["height"];

                        // 起動状態
                        if (env_name_list[env_id] == active_env_name)
                        {
                            output += OUTPUTTEXT["active"];
                        }
                        else
                        {
                            for (int i = 0; i < TEXTLENGTH["flag"]; i++)
                            {
                                output += OUTPUTTEXT["space"];
                            }
                        }

                        // 環境名
                        output += env_name_list[env_id];
                        while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"])
                        {
                            output += OUTPUTTEXT["space"];
                        }

                        // Pythonのバージョン
                        output += env_version_list[env_id];
                        while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"]+ TEXTLENGTH["space"])
                        {
                            output += OUTPUTTEXT["space"];
                        }

                        // 環境パス
                        output += env_path_list[env_id];
                        while (len(output) < 1 + TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"] + TEXTLENGTH["space"] + TEXTLENGTH["path"])
                        {
                            output += OUTPUTTEXT["space"];
                        }

                        output += OUTPUTTEXT["height"];
                        print(output);
                    }

                    // 最終行
                    output = OUTPUTTEXT["corner"];
                    for (int i = 0; i < TEXTLENGTH["flag"] + TEXTLENGTH["name"] + TEXTLENGTH["space"] + TEXTLENGTH["version"] + TEXTLENGTH["space"] + TEXTLENGTH["path"]; i++)
                    {
                        output += OUTPUTTEXT["width"];
                    }
                    output += OUTPUTTEXT["corner"];
                    print(output);
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
                    print("この引数は不正です");
                    help_text_env();
                }
            }
            else if (args[1] == "path")
            {
                auto names = envState.getEnvNameList();
                auto paths = envState.getEnvPathList();

                if (len(names) != len(paths))
                {
                    print("予期せぬエラーが発生しました.");
                }
                else
                {
                    // 探索
                    int idx = -1;
                    for (int i = 0; i < names.getSize(); i++)
                    {
                        if (names[i] == args[2])
                        {
                            idx = i;
                            break;
                        }
                    }

                    // 出力
                    if (idx < 0)
                    {
                        print("存在しない環境です.");
                    }
                    else
                    {
                        print(paths[idx]);
                    }
                }
            }
            else
            {
                print("引数が多すぎます。");
                help_text_env();
            }
        }
        else if (args[0] == "help")
        {
            help_text();
        }
        else if (args[0] == "install")
        {
            if (len(args) == 2)
            {
                if (args[1] == "--help")
                {
                    help_text_install();
                }
                else
                {
                    PythonVersion verson(args[1]);
                    if (verson.getMajor() < 0)
                    {
                        print("Pythonのバージョン指定がおかしいです");
                        help_text_install();
                    }
                    else
                    {
                        auto ret = pythonInstall(*(configs["venv_path"].getData<String>()), verson);
                        if (ret < 0)
                        {
                            print("すでにインストールされています");
                        }
                    }
                }
            }
            else
            {
                if (len(args) == 1)
                {
                    print("バージョン情報が足りません");
                }
                else if (len(args) > 2)
                {
                    print("引数が多すぎます。");
                }
                help_text_install();
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
                if (args[1] == "--help")
                {
                    help_text_path();
                }
                else if (args[1] == "add")
                {
                    addPath(".", *(configs["venv_path"].getData<String>()), envState);
                }
                else if (args[1] == "list")
                {
                    auto paths = pathList(*(configs["venv_path"].getData<String>()));
                    paths.del(0);
                    printList(paths);
                }
                else if (args[1] == "remove")
                {
                    removePath(".", *(configs["venv_path"].getData<String>()), envState);
                }
                else
                {
                    print("この引数は不正です");
                    help_text_path();
                }
            }
            else
            {
                if (args[1] == "--help")
                {
                    help_text_path();
                }
                else if (args[1] == "add")
                {
                    for (int i = 2; i < len(args); i++)
                    {
                        addPath(args[i], *(configs["venv_path"].getData<String>()), envState);
                    }
                }
                else if (args[1] == "remove")
                {
                    for (int i = 2; i < len(args); i++)
                    {
                        removePath(args[i], *(configs["venv_path"].getData<String>()), envState);
                    }
                }
                else
                {
                    print("この引数は不正です");
                    help_text_path();
                }
            }
        }
        else if (args[0] == "pip")
        {
            String env_name;

            if (len(args) == 1)
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
                if (len(args) == 2)
                {
                    print("引数が足りません");
                    help_text_pip();
                }
                else
                {
                    if (args[1] == "--help")
                    {
                        help_text_pip();
                    }
                    else if (args[1] == "add")
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
                    else if (args[1] == "remove")
                    {
                        if (envState.getEnvState(env_name))
                        {
                            String pip_cfg_path = *(configs["venv_path"].getData<String>()) + "env/" + env_name + "/pip.conf";
                            Dict<String, Dict<String, List<String>>> pip_cfgs;
                            readPipConfig(pip_cfg_path, pip_cfgs);
                            removePipConfig(args, pip_cfgs);
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
                                removePipConfig(args, pip_cfgs);
                                writePipConfig(pip_cfg_path, pip_cfgs);
                            }

                            String pip_cfg_path = *(configs["venv_path"].getData<String>()) + "config/pip.conf";
                            Dict<String, Dict<String, List<String>>> pip_cfgs;
                            readPipConfig(pip_cfg_path, pip_cfgs);
                            removePipConfig(args, pip_cfgs);
                            writePipConfig(pip_cfg_path, pip_cfgs);
                        }
                    }
                    else
                    {
                        print("この引数は不正です");
                        help_text_pip();
                    }
                }
            }
        }
        else if (args[0] == "python")
        {
            List<String> pythons = pythonList(*(configs["venv_path"].getData<String>()));
            if (len(args) == 1)
            {
                String default_python_version = "none";
                if (configs.exist("default_python_version"))
                {
                    default_python_version = *(configs["default_python_version"].getData<String>());
                }

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
                    if (args[1] != "--help")
                    {
                        print("この引数は不正です");
                    }
                    help_text_python();
                }
            }
            else if (len(args) == 3)
            {
                if (args[1] == "default")
                {
                    PythonVersion verson(args[2]);
                    if (verson.getMajor() < 0)
                    {
                        print("Pythonのバージョン指定がおかしいです");
                        help_text_python();
                    }
                    else
                    {
                        // 指定したバージョンが無い場合がなかったらインストール
                        pythonInstall(*(configs["venv_path"].getData<String>()), verson);

                        print("規定のPythonのバージョンを", args[2], "に指定しました");

                        // 設定ファイルに書き込み
                        configs["default_python_version"] = args[2];
                        env_config.write();
                    }
                }
                else if (args[1] == "install")
                {
                    PythonVersion verson(args[2]);
                    if (verson.getMajor() < 0)
                    {
                        print("Pythonのバージョン指定がおかしいです");
                        help_text_python();
                    }
                    else
                    {
                        auto ret = pythonInstall(*(configs["venv_path"].getData<String>()), verson);
                        if (ret < 0)
                        {
                            print("すでにインストールされています");
                        }
                    }
                }
                else
                {
                    if (args[1] != "--help")
                    {
                        print("この引数は不正です");
                    }
                    help_text_python();
                }
            }
            else
            {
                print("引数が多すぎます。");
                help_text_python();
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
                if (args[1] == "--help")
                {
                    help_text_remove();
                    return -1;
                }
                else
                {
                    auto flag = removeEnvConfirm(args[1], *(configs["venv_path"].getData<String>()), envState, false);
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
            }
            else if (len(args) == 3)
            {
                if (args[1] != "-y")
                {
                    help_text_remove();
                    return -1;
                }
                else
                {
                    auto flag = removeEnvConfirm(args[2], *(configs["venv_path"].getData<String>()), envState, true);
                    switch (flag)
                    {
                    case 0:
                        return 45;
                    case 1:
                        return 46;
                    case 2:
                        return 47;
                    case 3:
                        return 48;
                    default:
                        break;
                    }
                }
            }
            else
            {
                print("引数が多すぎます.");
            }

            help_text_remove();
            return -1;
        }
        else if (args[0] == "remove_environment")
        {
            removeEnvExecute(args[1], *(configs["venv_path"].getData<String>()));
        }
        else if (args[0] == "version")
        {
            if (len(args) > 1)
            {
                if (args[1] == "update")
                {
                    auto ret = updateLatestRelease(*(configs["venv_path"].getData<String>()), *(configs["default_python_version"].getData<String>()));
                    if (ret == 1)
                    {
                        return 50;
                    }
                }
                else
                {
                    if (args[1] != "--help")
                    {
                        print("この引数は不正です");
                    }
                    if (len(args) > 2)
                    {
                        print("引数が多すぎます.");
                    }
                    help_text_version();
                }
            }
            else
            {
                print(VENV_TOOL_VERSION);
                checkLatestRelease();
            }
        }
        else
        {
            print("引数に誤りがあります");
            help_text();
        }
    }

    return 0;
}
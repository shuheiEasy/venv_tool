#include "venv_tool.hpp"

namespace venv_tool
{
    using namespace dataObject;
    using namespace fileSystem;
    using namespace logSystem;

    bool activateEnv(String env_name, String &venv_path, EnvState &state)
    {
        String cmd = venv_path + "env/" + env_name + "/bin/activate";
        print(cmd);

        String current_env_name = "";
        if (state.getEnvState(current_env_name))
        {
            if (current_env_name == env_name)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    void addConfig(List<String> args, JsonFile &config_file)
    {
        // 辞書取得
        Dict<String, Any> &configs = config_file.getDict();

        // 変数登録
        for (int i = 0; i < args.getSize(); i++)
        {
            auto arg = args[i];
            auto key_and_values = arg.split("=");

            // 入力が不正な場合
            if (len(key_and_values) != 2)
            {
                continue;
            }

            String key = key_and_values[0];
            String values = key_and_values[1];

            auto value_list = values.split(",");

            if (configs.exist(key))
            {
                if (type(configs[key]) == "List")
                {
                    List<Any> *list = configs[key].getData<List<Any>>();
                    for (int j = 0; j < len(value_list); j++)
                    {
                        list->append(Any(value_list[j]));
                    }
                }
                else
                {
                    List<Any> list;
                    list.append(configs[key]);
                    for (int j = 0; j < len(value_list); j++)
                    {
                        list.append(Any(value_list[j]));
                    }
                    configs[key] = Any(list);
                }
            }
            else
            {
                if (len(value_list) == 1)
                {
                    configs[key] = value_list[0];
                }
                else
                {
                    configs[key] = value_list;
                }
            }
        }

        // ファイル書き込み
        config_file.write();
    }

    void addPath(String path, String &venv_path, EnvState &state)
    {
        String active_env_name;
        if (state.getEnvState(active_env_name))
        {
            File dir(path);
            auto exist_paths = pathList(venv_path);
            bool write_flag = true;

            for (int i = 0; i < exist_paths.getSize(); i++)
            {
                File buffer(exist_paths[i]);
                if (dir.getPath() == buffer.getPath())
                {
                    write_flag = false;
                    break;
                }
            }

            if (write_flag)
            {
                String pth_file = venv_path + "env/" + active_env_name + "/lib";

                pth_file = getDirList(pth_file)[0].getPath();
                pth_file += "site-packages/paths.pth";

                TextFile text_file(pth_file);
                if (!text_file.exists())
                {
                    text_file.touch();
                }

                text_file.writeline(dir.getPath());
            }
            else
            {
                print("すでにそのパスは追加されています");
            }
        }
        else
        {
            print("仮想環境に入っていません");
        }
    }

    void addPipConfig(List<String> args, Dict<String, Dict<String, List<String>>> &pip_cfgs)
    {
        String loading_order = "[global]";
        for (int i = 2; i < len(args); i++)
        {
            auto key_and_value = args[i].split("=");
            if (len(key_and_value) != 2)
            {
                continue;
            }

            List<String> values, buffer;
            buffer = key_and_value[1].split(",");

            for (int j = 0; j < len(buffer); j++)
            {
                bool no_exist = true;
                for (int k = 0; k < len(values); k++)
                {
                    if (values[k] == buffer[j])
                    {
                        no_exist = false;
                        break;
                    }
                }
                if (no_exist)
                {
                    values.append(buffer[j]);
                }
            }

            if (buffer.getSize() > 0)
            {
                buffer.clear();
            }

            if (pip_cfgs[loading_order].getData(key_and_value[0], buffer) == 0)
            {
                for (int j = 0; j < len(buffer); j++)
                {
                    bool no_exist = true;
                    for (int k = 0; k < len(values); k++)
                    {
                        if (values[k] == buffer[j])
                        {
                            no_exist = false;
                            break;
                        }
                    }

                    if (no_exist)
                    {
                        values.append(buffer[j]);
                    }
                }
            }
            pip_cfgs[loading_order][key_and_value[0]] = values;
        }
    }

    void checkEnv(fileSystem::JsonFile &cfg_json_file)
    {
        auto &cfg = cfg_json_file.getDict();

        String *venv_path = cfg["venv_path"].getData<String>();
        File venv_space(*venv_path);

        if (*venv_path != venv_space.getPath())
        {
            cfg["venv_path"] = venv_space.getPath();
            cfg_json_file.write();
        }

        if (!venv_space.exists())
        {
            venv_space.mkdir();
        }

        String path = venv_space.getPath() + "Python";
        File pydir(path);
        if (!pydir.exists())
        {
            pydir.mkdir();
        }

        path = venv_space.getPath() + "env";
        File envdir(path);
        if (!envdir.exists())
        {
            envdir.mkdir();
        }
    }

    int checkLatestRelease()
    {
        fileSystem::JsonFile json;

        // 情報取得
        if (getLatestRelease(json) != 0)
        {
            return -1;
        }

        // 情報解析
        PythonVersion release_version(json["name"].getData<String>()->slice(1, 10));
        PythonVersion tool_version(String(VENV_TOOL_VERSION));

        if (release_version > tool_version)
        {
            logSystem::print("新しいバージョンがあります (", tool_version.getVersion(), "->", release_version.getVersion(), ")");
        }

        return 0;
    }

    List<String> command(dataObject::String cmd)
    {
        FILE *fp;
        // NULLか判定
        if ((fp = popen(cmd.getChar(), "r")) == NULL)
        {
            exit(1);
        }

        char buf[1024];
        List<String> ret;
        while (fgets(buf, sizeof(buf), fp) != NULL)
        {
            String bufs = buf;
            List<String> buffer = bufs.split("\n");
            for (int i = 0; i < len(buffer); i++)
            {
                if (len(buffer[i]) > 0)
                {
                    ret.append(buffer[i]);
                }
            }
        }

        pclose(fp);

        return ret;
    }

    int createEnv(String env_name, String venv_path, PythonVersion python_version)
    {
        if (python_version.getMajor() < -1)
        {
            print("Pythonのバージョン指定がおかしいです");
            return -1;
        }

        pythonInstall(venv_path, python_version);

        // 仮想環境の一覧
        String envs_path = venv_path + "env/";

        // カレントディレクトリ取得
        String current_path = getCurrentDir();

        // 仮想環境の一覧フォルダへ移動
        moveCurrentDir(envs_path);

        // 仮想環境の作成
        String cmd = venv_path + "Python/" + python_version.getVersion() + "/bin/python3 -m venv ";
        cmd += env_name;
        system(cmd.getChar());

        // Pipの関連設定
        Dict<String, Dict<String, List<String>>> pip_cfgs;
        String pip_cfg_path = venv_path + "config/pip.conf";
        readPipConfig(pip_cfg_path, pip_cfgs);

        pip_cfg_path = venv_path + "env/" + env_name + "/pip.conf";
        writePipConfig(pip_cfg_path, pip_cfgs);

        // 元の場所へ帰る
        moveCurrentDir(current_path);

        return 0;
    }

    bool downloadZip(const char *url, const char *savePath)
    {
        CURL *curl;
        CURLcode res;
        bool success = false; // ダウンロードの成功フラグ

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl)
        {
            FILE *fp = fopen(savePath, "wb"); // ファイルをバイナリ書き込みモードで開く

            if (fp)
            {
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToFileCallback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

                // リダイレクトをフォローするオプションを設定
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                // ダウンロード進捗情報を表示するコールバックを設定
                // curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progressCallback);
                // curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

                res = curl_easy_perform(curl);

                fclose(fp);

                if (res == CURLE_OK)
                {
                    std::cout << "ZIP file downloaded successfully." << std::endl;
                    success = true; // ダウンロード成功
                }
                else
                {
                    std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
                }
            }
            else
            {
                std::cerr << "Failed to open file for writing." << std::endl;
            }

            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();
        return success; // ダウンロードの成功フラグを返す
    }

    int getLatestRelease(fileSystem::JsonFile &json)
    {
        int ret = -1;

        // GitHubのリリース情報を取得するためのAPIエンドポイント
        const std::string API_ENDPOINT = "https://api.github.com/repos/shuheiEasy/venv_tool/releases/latest";

        CURL *curl;
        CURLcode res;

        // libcurlの初期化
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init();

        if (curl)
        {
            // HTTPリクエストの設定
            std::string response;
            curl_easy_setopt(curl, CURLOPT_URL, API_ENDPOINT.c_str());
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Bing");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // HTTPリクエストの実行
            res = curl_easy_perform(curl);

            // リクエストが成功したかどうかを確認
            if (res != CURLE_OK)
            {
                std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            }
            else
            {
                // レスポンスの表示
                json.setData(response.c_str());
                ret = 0;
            }

            // libcurlのクリーンアップ
            curl_easy_cleanup(curl);
        }

        // libcurlの終了処理
        curl_global_cleanup();

        return ret;
    }

    void setEnvironmentPath(String env_name, String append_dir_path, Dict<String, String> &cfg)
    {
        // 仮想環境のlibフォルダへ移動
        moveCurrentDir(cfg["venv_path"] + "env/" + env_name + "/lib");

        // 下層のフォルダへ移動
        auto dir_list = getDirList(".");
        if (len(dir_list) > 1)
        {
            print("ERROR!!!");
        }
        else if (len(dir_list) == 0)
        {
            return;
        }
        moveCurrentDir(dir_list[0].getPath());

        // PATHを通す
        TextFile path_file("site-packages/envPath.pth");
        if (!path_file.exists())
        {
            path_file.touch();
        }

        path_file.writeline(append_dir_path);
    }

    int updateLatestRelease(String venv_path, String python_version)
    {
        fileSystem::JsonFile json;

        // 情報取得
        if (getLatestRelease(json) != 0)
        {
            return -1;
        }

        // 情報解析
        PythonVersion release_version(json["name"].getData<String>()->slice(1, 10));
        PythonVersion tool_version(String(VENV_TOOL_VERSION));
        String downloadURL = *(json["assets"].getData<List<Any>>()->get(0).getData<Dict<String, Any>>()->at("browser_download_url").getData<String>());

        // 更新があるときだけアップデート
        if (release_version > tool_version)
        {
            // ダウンロードフォルダ作成
            auto tmp_path = venv_path + "tmp";
            File tmp_dir(tmp_path);
            tmp_dir.mkdir();

            // カレントディレクトリ取得
            auto current_dir = getCurrentDir();

            // 移動
            moveCurrentDir(tmp_path);

            // Download
            auto ret = downloadZip(downloadURL.c_str(), "save.zip");
            if (!ret)
            {
                return -2;
            }

            // 解凍
            system("unzip -q save.zip");

            // 移動
            moveCurrentDir("venv_tool");

            // インストールコマンド作成
            String cmd = "./installer.sh --update --install_space ";
            cmd += venv_path + " --python " + python_version;
            system(cmd.c_str());
            logSystem::print(cmd);

            // 移動
            moveCurrentDir(current_dir);

            // 削除
            tmp_dir.remove(true);
            return 1;
        }

        return 0;
    }

    List<String> pathList(String &venv_path)
    {
        String cmd = "python3 ";
        cmd += venv_path + "bin/getPyPath.py";
        return command(cmd);
    }

    void printList(List<String> list)
    {
        String output = "";
        for (int i = 0; i < len(list); i++)
        {
            if (i > 0)
            {
                output += " ";
            }
            output += list[i];
        }
        print(output);
    }

    int pythonInstall(String venv_path, PythonVersion version)
    {
        if (version.getMajor() < 0)
        {
            print("Pythonのバージョン指定がおかしいです");
            return -1;
        }

        // Pythonのインストール先
        auto python_install_path = venv_path + "Python/" + version.getVersion();

        File python_install_dir(python_install_path);
        if (python_install_dir.exists())
        {
            return -1;
        }

        // カレントディレクトリ取得
        auto current_dir = getCurrentDir();

        // URL作成
        String python_name = "Python-";
        python_name += version.getVersion();
        String python_file = python_name + ".tgz";

        String path = "https://www.python.org/ftp/python/";
        path += version.getVersion();
        path += "/";
        path += python_file;

        // ダウンロードフォルダ作成
        auto tmp_path = venv_path + "tmp";
        File tmp_dir(tmp_path);
        tmp_dir.mkdir();

        // ダウンロードコマンド作成
        String cmd = "wget -P ";
        cmd += tmp_path;
        cmd += " ";
        cmd += path;

        // Download
        system(cmd.getChar());

        // 解凍コマンド作成
        cmd = "tar xvzf ";
        cmd += tmp_path + "/";
        cmd += python_file;
        cmd += " -C ";
        cmd += tmp_path;

        // 解凍
        system(cmd.getChar());

        print(getFileList(tmp_path));

        // 移動
        moveCurrentDir(tmp_path + "/" + python_name);

        // configure
        cmd = "./configure";
        if (version.getMinor() <= 7)
        {
            cmd += " CFLAGS=-O2";
        }
        cmd += " --prefix=";
        cmd += python_install_path;
        system(cmd.getChar());

        // make
        system("make");

        // make
        system("make install");

        // 移動
        moveCurrentDir(current_dir);

        // 削除コマンド作成
        cmd = "rm -rf ";
        cmd += tmp_path + "/";

        // 削除
        system(cmd.getChar());

        return 0;
    }

    List<String> pythonList(String &venv_path)
    {
        auto path = venv_path + "Python";
        List<String> ret;
        List<PythonVersion> pylist;

        auto list = getDirList(path);

        for (int i = 0; i < len(list); i++)
        {
            PythonVersion buf(list[i].getName());
            pylist.append(buf);
        }

        for (int i = 0; i < len(pylist); i++)
        {
            for (int j = i + 1; j < len(pylist); j++)
            {
                if (pylist[i] > pylist[j])
                {
                    auto tmp = pylist[i];
                    pylist[i] = pylist[j];
                    pylist[j] = tmp;
                }
            }
        }

        for (int i = 0; i < len(pylist); i++)
        {
            ret.append(pylist[i].getVersion());
        }

        return ret;
    }

    void readPipConfig(String pip_cfg_path, Dict<String, Dict<String, List<String>>> &cfg)
    {
        TextFile cfgFile(pip_cfg_path);

        if (!cfgFile.exists())
        {
            cfgFile.touch();
        }
        else
        {
            auto lines = cfgFile.readlines();
            String loading_order = "";
            String key = "";
            List<String> values;

            for (int i = 0; i < len(lines); i++)
            {
                // Loading order
                int loading_order_flag = 0;
                for (int j = 0; j < len(lines[i]); j++)
                {
                    if (lines[i][j] == "[")
                    {
                        loading_order_flag++;
                    }
                    if (lines[i][j] == "]")
                    {
                        loading_order_flag++;
                    }
                }

                if (loading_order_flag == 2)
                {
                    if (key != "")
                    {
                        // 辞書に追加
                        cfg[loading_order][key] = values;
                        key = "";
                        values.clear();
                    }
                    Dict<String, List<String>> dict;
                    String buf = "";
                    for (int j = 0; j < len(lines[i]); j++)
                    {
                        if (lines[i][j] == " ")
                        {
                            continue;
                        }
                        buf += lines[i][j];
                    }
                    cfg[buf] = dict;
                    loading_order = buf;
                }
                else
                {
                    // 値とキーに分離
                    auto line_list = lines[i].split("=");

                    if (len(line_list) == 0)
                    {
                        auto list_buf = line_list[0].split(" ");
                        if (len(list_buf) > 0)
                        {
                            values.extend(list_buf);
                        }
                    }
                    else if (len(line_list) == 2)
                    {

                        // 辞書に追加
                        if (key != "")
                        {
                            cfg[loading_order][key] = values;
                            key = "";
                            values.clear();
                        }

                        // キーがない時
                        Dict<String, List<String>> dict;
                        if (cfg.getData(loading_order, dict) != 0)
                        {
                            cfg[loading_order] = dict;
                        }

                        String buf = "";
                        for (int j = 0; j < len(line_list[0]); j++)
                        {
                            if (line_list[0][j] == "[" || line_list[0][j] == "]" || line_list[0][j] == " ")
                            {
                                continue;
                            }
                            buf += line_list[0][j];
                        }
                        key = buf;
                        values.clear();
                        buf = "";
                        for (int j = 0; j < len(line_list[1]); j++)
                        {
                            if (line_list[1][j] == "[" || line_list[1][j] == "]")
                            {
                                continue;
                            }
                            else if (line_list[1][j] == " ")
                            {
                                if (buf != "")
                                {
                                    values.append(buf);
                                }
                                buf = "";
                            }
                            else
                            {
                                buf += line_list[1][j];
                            }
                        }
                        values.append(buf);
                    }
                }
            }

            // 辞書に追加
            if (key != "")
            {
                cfg[loading_order][key] = values;
            }
        }
    }

    int removeEnvConfirm(String env_name, String &venv_path, EnvState &state, bool yes_flag)
    {
        String path = venv_path + "env/" + env_name;
        File env_dir(path);

        if (env_dir.isdir())
        {
            int letter;
            String option;
            bool remove_flag = yes_flag;
            char text[2];
            text[1] = '\0';

            if (!remove_flag)
            {
                print(env_name, "環境を本当に削除しますか？[y]");
                while ((letter = getchar()) != EOF)
                {
                    text[0] = (char)letter;
                    String moji(text);
                    if (moji == "\n")
                    {
                        break;
                    }
                    else
                    {
                        option += moji;
                    }
                }

                if (option == "Y" || option == "y" || option == "Yes" || option == "yes"){
                    remove_flag=true;
                }
            }

            if (remove_flag)
            {
                String current_env_name = "";
                if (state.getEnvState(current_env_name))
                {
                    if (current_env_name == env_name)
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                print(env_name, "環境の削除を取りやめました。");
                return 2;
            }
        }
        else
        {
            print(env_name, "環境は存在しません!");
        }
        return 3;
    }

    void removeEnvExecute(String env_name, String &venv_path)
    {
        String path = venv_path + "env/" + env_name;
        String cmd = "rm -rf ";
        cmd += path;

        system(cmd.getChar());
        print(env_name, "環境を削除しました。");
    }

    void removePath(String path, String &venv_path, EnvState &state)
    {
        String active_env_name;
        if (state.getEnvState(active_env_name))
        {
            // パスの生成
            String pth_file = venv_path + "env/" + active_env_name + "/lib";
            pth_file = getDirList(pth_file)[0].getPath();
            pth_file += "site-packages/paths.pth";

            // Pathファイル
            TextFile path_file(pth_file);
            if (!path_file.exists())
            {
                path_file.touch();
            }

            // Path確認
            List<String> path_list = path_file.readlines();
            File dir(path);
            bool write_flag = false;
            int path_id = 0;

            while (path_id < len(path_list))
            {
                // 空なら削除
                if (len(path_list[path_id]) == 0)
                {
                    path_list.del(path_id);
                    continue;
                }

                // パスの確認
                if (!write_flag)
                {
                    File buffer(path_list[path_id]);
                    if (dir.getPath() == buffer.getPath())
                    {
                        write_flag = true;
                        path_list.del(path_id);
                        continue;
                    }
                }

                path_id++;
            }

            if (write_flag)
            {
                path_file.writelines(path_list, WRITEMODE);
            }
            else
            {
                print("そのパスは存在しません");
            }
        }
        else
        {
            print("仮想環境に入っていません");
        }
    }

    void removePipConfig(List<String> args, Dict<String, Dict<String, List<String>>> &pip_cfgs)
    {
        String loading_order = "[global]";
        for (int i = 2; i < len(args); i++)
        {
            pip_cfgs[loading_order].del(args[i]);
        }
    }

    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        ((std::string *)userp)->append((char *)contents, size * nmemb);
        return size * nmemb;
    }

    size_t WriteToFileCallback(void *ptr, size_t size, size_t nmemb, void *userdata)
    {
        size_t written = fwrite(ptr, size, nmemb, (FILE *)userdata);
        return written;
    }

    void writePipConfig(dataObject::String pip_cfg_path, Dict<String, Dict<String, List<String>>> pip_cfgs)
    {
        TextFile cfgFile(pip_cfg_path);
        if (!cfgFile.exists())
        {
            cfgFile.touch();
        }

        List<String> orders = pip_cfgs.getKeys();
        for (int j = 0; j < len(pip_cfgs); j++)
        {
            List<String> keys = pip_cfgs[orders[j]].getKeys();
            if (j == 0)
            {
                cfgFile.writeline(orders[j], WRITEMODE);
            }
            else
            {
                cfgFile.writeline(orders[j], APPENDMODE);
            }
            for (int i = 0; i < len(pip_cfgs[orders[j]]); i++)
            {
                String text = keys[i];
                text += " = ";

                auto values = pip_cfgs[orders[j]][keys[i]];
                for (int k = 0; k < len(values); k++)
                {
                    if (k > 0)
                    {
                        text += " ";
                    }
                    text += values[k];
                }

                cfgFile.writeline(text, APPENDMODE);
            }
        }
    }

}
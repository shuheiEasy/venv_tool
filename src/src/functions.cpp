#include "venv_tool.hpp"

namespace venv_tool
{
    using namespace dataObject;
    using namespace fileSystem;
    using namespace logSystem;

    bool activateEnv(String env_name, Dict<String, String> &cfg, EnvState &state)
    {
        String cmd = cfg["venv_path"] + "env/" + env_name + "/bin/activate";
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

    void checkEnv(dataObject::String env_cfg_path, Dict<String, String> &cfg)
    {
        auto venv_path = cfg["venv_path"];
        File venv_space(venv_path);
        if (venv_path != venv_space.getPath())
        {
            cfg["venv_path"] = venv_space.getPath();
            TextFile cfg_file(env_cfg_path);
            cfg_file.write("", WRITEMODE, false);
            List<String> cfg_keys = cfg.getKeys();
            for (int i = 0; i < len(cfg_keys); i++)
            {
                auto key = cfg_keys[i];
                cfg_file.writeline(key + " = " + cfg[key]);
            }
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
        String current_path = getCurrentDirectory();

        // 仮想環境の一覧フォルダへ移動
        moveCurrentDirectory(envs_path);

        // 仮想環境の作成
        String cmd = venv_path + "Python/" + python_version.getVersion() + "/bin/python3 -m venv ";
        cmd += env_name;
        system(cmd.getChar());

        // 元の場所へ帰る
        moveCurrentDirectory(current_path);

        return 0;
    }

    String getCurrentDirectory(void)
    {
        char buf[1024];
        getcwd(buf, 1024);
        return String(buf);
    }

    void help_text(void)
    {
        print("venv_tool version ", VENV_TOOL_VERSION);
    }

    void moveCurrentDirectory(String path)
    {
        chdir(path.getChar());
    }

    void setEnvironmentPath(String env_name, String append_dir_path, Dict<String, String> &cfg)
    {
        // 仮想環境のlibフォルダへ移動
        moveCurrentDirectory(cfg["venv_path"] + "env/" + env_name + "/lib");

        // 下層のフォルダへ移動
        FileExplorer fe(".");
        auto dir_list = fe.getDirList();
        if (len(dir_list) > 1)
        {
            print("ERROR!!!");
        }
        else if (len(dir_list) == 0)
        {
            return;
        }
        moveCurrentDirectory(dir_list[0].getPath());

        // PATHを通す
        TextFile path_file("site-packages/envPath.pth");
        if (!path_file.exists())
        {
            path_file.touch();
        }

        path_file.writeline(append_dir_path);
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
        if (version.getMajor() < -1)
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
        auto current_dir = getCurrentDirectory();

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

        FileExplorer tmp_dir_fe(tmp_path);
        print(tmp_dir_fe.getFileList());

        // 移動
        moveCurrentDirectory(tmp_path + "/" + python_name);

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
        moveCurrentDirectory(current_dir);

        // 削除コマンド作成
        cmd = "rm -rf ";
        cmd += tmp_path + "/";

        // 削除
        system(cmd.getChar());

        return 0;
    }

    List<String> pythonList(Dict<String, String> &cfg)
    {
        auto path = cfg["venv_path"] + "Python";
        List<String> ret;
        List<PythonVersion> pylist;

        FileExplorer fe(path);
        auto list = fe.getDirList();

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

    void readConfig(String cfg_file_path, Dict<String, String> &cfg)
    {
        TextFile cfgFile(cfg_file_path);
        auto lines = cfgFile.readlines();

        for (int i = 0; i < len(lines); i++)
        {
            // 空白削除
            auto line_list = lines[i].split(" ");
            String buf;
            for (int i = 0; i < len(line_list); i++)
            {
                buf += line_list[i];
            }

            // 値とキーに分離
            line_list = buf.split("=");

            if (len(line_list) != 2)
            {
                continue;
            }

            // 辞書に追加
            cfg[line_list[0]] = line_list[1];
        }
    }

    void writeConfig(String cfg_file_path, Dict<String, String> cfg)
    {
        TextFile cfgFile(cfg_file_path);

        List<String> keys = cfg.getKeys();
        for (int i = 0; i < len(cfg); i++)
        {
            String text = keys[i];
            text += " = ";
            text += cfg[keys[i]];

            if (i == 0)
            {
                cfgFile.writeline(text, WRITEMODE);
            }
            else
            {
                cfgFile.writeline(text, APPENDMODE);
            }
        }
    }
}
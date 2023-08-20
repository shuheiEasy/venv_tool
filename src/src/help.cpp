#include "venv_tool_help.hpp"

using namespace dataObject;
using namespace fileSystem;
using namespace logSystem;

namespace venv_tool
{
    void help_text(void)
    {
        // タイトル
        print("venv_tool version:", VENV_TOOL_VERSION);

        // ツールの説明
        print("\nvenvを使いやすくするツールです.");

        // 使い方の説明
        print("使い方: venv <COMMAND> [OPTION] [ARGS]...");

        // コマンド一覧
        print("コマンド一覧");
        print(" activate     仮想環境に入る.");
        print(" configure    ツールを設定する");
        print(" deactivate   仮想環境を出る.");
        print(" env          仮想環境を確認する.");
        print(" install      Pythonをインストールする.");
        print(" path         Pythonのパスを設定する.");
        print(" pip          Pipの設定をする.");
        print(" python       Pythonを設定する.");
        print(" remove       仮想環境を削除する.");
        print(" version      バージョンを確認する.");
        print("\n各コマンドの詳細は venv <COMMAND> --help で確認してください.");
    }
    void help_text_activate(void)
    {
        print("使い方: venv activate <NAME>");
        print("　　　  NAME:起動する環境名");
        print("\n指定した仮想環境を起動する.");
    }
    void help_text_configure(void)
    {
        print("使い方: venv configure [COMMAND]");
        print("venv_toolで使う環境変数を操作する.");

        print("\nコマンド一覧");
        print(" add    venv_toolの環境変数を設定する.");
        print("        使い方: venv configure add <KEY>=<VALUE>");
        print(" show   venv_toolの環境変数を確認する.");
    }
    void help_text_create(void)
    {
        print("使い方: venv create <NAME> [VERSION]");
        print("　　　  NAME: 作成する環境名");
        print("　　　  VERSION: Pythonのバージョン(省略した場合はデフォルトのバージョン)");
        print("\n仮想環境を作成する.");
    }
    void help_text_deactivate(void)
    {
        print("使い方: venv deactivate");
        print("起動している仮想環境を終了する.");
    }
    void help_text_env(void)
    {
        print("使い方: venv env [COMMAND]");
        print("venv_toolで管理している仮想環境を確認する.");

        print("\nコマンド一覧");
        print(" list   venv_toolで管理している仮想環境の名前の一覧を表示する.");
        print(" path   venv_toolで管理している仮想環境の絶対パスの一覧を表示する.");
        print(" show   venv_toolで管理している仮想環境の詳細を確認する.");
        print(" state  現在の環境を表示する.");
    }
    void help_text_install(void)
    {
        print("使い方: venv install <VERSION>");
        print("　　　  VERSION: 作成する環境名");
        print("venv_toolで使うPythonをインストールする.");
    }
    void help_text_path(void)
    {
        print("使い方: venv path [COMMAND]");
        print("現在の環境のPythonのパスを設定する.");

        print("\nコマンド一覧");
        print(" add     PYTHONPATHにパスを追加する.");
        print("         使い方: venv python add <PATH>...");
        print(" list    現在のPythonのパスを確認する.");
        print(" remove  PYTHONPATHからパスを削除する.");
        print("         使い方: venv python remove <PATH>...");
    }
    void help_text_pip(void)
    {
        print("使い方: venv pip [COMMAND]");
        print("現在の環境のPipを設定する.");

        print("\nコマンド一覧");
        print(" add     Pipの設定を追加する.");
        print("         使い方: venv pip add <KEY>=<VALUE>...");
        print(" remove  Pipの設定を削除する.");
        print("         使い方: venv pip remove <KEY>...");
    }
    void help_text_python(void)
    {
        print("使い方: venv python [COMMAND]");
        print("venv_toolで使うPythonを管理する.");

        print("\nコマンド一覧");
        print(" default  venv_toolで使うPythonの既定のバージョンを設定する.");
        print("          使い方: venv python default <VERSION>");
        print(" install  venv_toolで使うPythonをインストールする.");
        print("          使い方: venv python install <VERSION>");
        print(" list     venv_toolで管理するPythonのバージョンのリスト.");
    }
    void help_text_remove(void)
    {
        print("使い方: venv remove <NAME>");
        print("　　　  NAME:削除する環境名");
        print("\n指定した仮想環境を削除する.");
    }
    void help_text_version(void)
    {
        print("使い方: venv version [COMMAND]");
        print("venv_toolのバージョンを表示する.");

        print("\nコマンド一覧");
        print(" update  venv_toolを最新版に更新する.");
    }
}
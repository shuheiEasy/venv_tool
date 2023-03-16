# CMakeのバージョンを設定
cmake_minimum_required(VERSION 3.13)

# プロジェクト名と使用する言語を設定
project(venv_tool LANGUAGES CXX VERSION 1.2.0)

# 静的リンクへ変更
set(STATIC_LINK_FLAG ON CACHE BOOL ON FORCE)

# azagonディレクトリ
add_subdirectory(azagon)

# 環境変数ファイルのパスの変数
set(VenvToolConfigFile . CACHE PATH "config file path")

# configファイル
configure_file (
  "include/venv_tool_config.h.in"
  "${PROJECT_BINARY_DIR}/include/venv_tool_config.h"
  )
configure_file (
  "config/env.json"
  "${PROJECT_BINARY_DIR}/config/env.json"
  )
configure_file (
  "scripts/complementHead.sh"
  "${PROJECT_BINARY_DIR}/scripts/complementHead.sh"
  )
configure_file (
  "scripts/venv_tool"
  "${PROJECT_BINARY_DIR}/bin/venv_tool"
  )
# シェルスクリプトのコピー
file(COPY "scripts/complementMain.sh" DESTINATION ${PROJECT_BINARY_DIR}/scripts)

# Pythonファイルのコピー
file(COPY "scripts/getPyPath.py" DESTINATION ${PROJECT_BINARY_DIR}/scripts)

# インクルードフォルダ追加
include_directories(include)
include_directories(${azagon_INCLUDE_DIR})
include_directories(${PROJECT_BINARY_DIR}/include)

# C++ファイル取得
file(GLOB_RECURSE source_list "src/*.cpp")

# 実行ファイルをソースから作成
add_executable(${PROJECT_NAME}_exe ${source_list})

# リンク
target_link_libraries(${PROJECT_NAME}_exe ${azagon_LIBS})

# インストール
install(TARGETS ${PROJECT_NAME}_exe RUNTIME DESTINATION bin)
install(FILES ${PROJECT_BINARY_DIR}/config/env.json DESTINATION config PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
install(FILES ${PROJECT_BINARY_DIR}/bin/venv_tool DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
install(FILES ${PROJECT_BINARY_DIR}/scripts/getPyPath.py DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
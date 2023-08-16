#ifndef VENV_TOOL_HELP_H
#define VENV_TOOL_HELP_H

#include <dataObject/dataObject.hpp>
#include <logSystem/logSystem.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>

#include "venv_tool_config.h"

namespace venv_tool
{
    // Help文出力
    void help_text(void);
    void help_text_activate(void);
    void help_text_configure(void);
    void help_text_create(void);
    void help_text_deactivate(void);
    void help_text_env(void);
    void help_text_install(void);
    void help_text_path(void);
    void help_text_pip(void);
    void help_text_python(void);
    void help_text_remove(void);
    void help_text_version(void);
};

#endif
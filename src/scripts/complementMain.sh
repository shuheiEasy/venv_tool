_venv_complement() {
    local cur=${COMP_WORDS[COMP_CWORD]}
    local env_list_path="$VENVSPACE/envs"

    local env_list=""
    local d_list=($env_list_path/*/)

    if [ ${#d_list[@]} -gt 0 ]; then
        local env_list_first_flag=true
        local env_path_list=($($tool_exe_path env name))

        for e_p in ${env_path_list[@]}; do
            if "${env_list_first_flag}"; then
                env_list="$e_p"
                env_list_first_flag=false
            else
                env_list="$env_list $e_p"
            fi
        done
    fi

    local py_list=($($tool_exe_path python list))
    local py_list_text=""
    local py_list_first_flag=true
    for pl in ${py_list[@]}; do
        if "${py_list_first_flag}"; then
            py_list_text="$pl"
            py_list_first_flag=false
        else
            py_list_text="$py_list_text $pl"
        fi
    done

    case "$COMP_CWORD" in
    "1")
        COMPREPLY=($(compgen -W "activate create deactivate env install python remove version" -- $cur))
        ;;
    "2")
        case "${COMP_WORDS[1]}" in
        "activate" | "remove")
            COMPREPLY=($(compgen -W "$env_list" -- $cur))
            ;;
        "list")
            COMPREPLY=($(compgen -W "name path" -- $cur))
            ;;
        "python")
            COMPREPLY=($(compgen -W "default list" -- $cur))
            ;;
        esac
        ;;
    "3")
        case "${COMP_WORDS[1]}" in
        "create")
            COMPREPLY=($(compgen -W "$py_list_text" -- $cur))
            ;;

        "python")
            case "${COMP_WORDS[2]}" in
            "default")
                COMPREPLY=($(compgen -W "$py_list_text" -- $cur))
                ;;
            esac
            ;;
        esac
        ;;
    esac
}

venv() {
    source $tool_shell_path $@
}

complete -F _venv_complement venv

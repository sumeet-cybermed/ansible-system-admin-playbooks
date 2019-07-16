#!/bin/bash


function confirmYN () {

    # call with a prompt string or use a default
    PROMPTYN="Are you sure? [y/N]"
    read -r -p "${1:-${PROMPTYN}} " response
    case $response in
        [yY][eE][sS]|[yY])
            true
            ;;
        *)
            false
            ;;
    esac
}

function promptUserBeforeRunningPlaybook()
{
    PROMPTUSER="Run Playbook With Parameters: -p "${PLAYBOOK}"  -a "${APP}" -c "${CONFIG}" -s "${SITE}" -v "${VERBOSITY}" -ip "${IP}" -ss "${SITESALT}" "
    echo "
${PROMPTUSER}" "$@"
    if confirmYN
    then
        echo "User Confirmed - continuing..."
    else
        echo "User Cancelled - exiting ..."
        exit 0;
    fi
}

function promptUserBeforeRunningAdHocCommand()
{
    PROMPTUSER="$0 Run with parameters: -i ${INVENTORY} -b -a '"${RUN_COMMAND}"' "
    echo "
${PROMPTUSER}" "$@"
    if confirmYN
    then
        echo "User Confirmed - continuing..."
    else
        echo "User Cancelled - exiting ..."
        exit 0;
    fi
}


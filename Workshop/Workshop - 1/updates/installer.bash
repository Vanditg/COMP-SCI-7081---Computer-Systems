#!/bin/bash

# stage 0 default installer for assignments and workshops
# builds generic assignment / workshop startup files


# The Plan:
# . installer.bash runs updater.bash from newest updates/YYMMDD-HHIISS-name.zip
#   . all updates/*zip must use the same assign or workshop name
#   . all output is logged in updates/log
# . updater.bash creates default directory structure and updates startup files
#   . the top level should be:
#     Makefile, bin, includes, lib, tests, toedit, updates
#   . top level also includes a copy of everything a student changes in toedit 
#     If missing from top level, copy from toedit
#     If present at top level, do nothing ...
# . updater.bash ensures all executables are executable, this may involve removing quarantine flags
# . a student svn directory must contain:
#   . at the top level
#     Makefile, updates, and any files students are asked to edit
#   . in the updates directory
#     log, installer.bash and *-*-*.zip files

# remove all access for others
chmod -R go= *

# setup colours for interative output
# if using an xterm - use headings and colour coded test results
if [ "${TERM:0:5}" == "xterm" -o "${TERM:0:2}" == "vt" ] ; then
    colorboldblack="\x1b[1;7;30m"
    colorboldgreen="\x1b[1;7;32m"
    colorboldred="\x1b[1;7;31m"
    undocolor="\x1b[0m"
else
    colorboldblack=
    colorboldgreen=
    colorboldred=
    undocolor=
fi

# checksum changed files
check_changed_files()
{
    # checksum all files modified since last change to updates/log
    find . -type f -newer updates/log -exec shasum \{} \; | grep -v -e './.changes' -e './.reminders' -e './updates/log' > .changes 2> /dev/null
    if [ -s .changes ]
    then
        # record summary in updates/log
        echo -n "${now} Modified: " >> updates/log
        cat .changes | base64 >> updates/log
    fi

    # clean up ...
    rm -f .changes
}

# record start of this run
now=`date +%y%m%d-%H%M%S`
check_changed_files
echo "${now} make ${@}" >> updates/log
chmod 600 updates/log

# reminder every hour - svn udpate / svn commit
reminders_svn()
{
    if [ -f .reminders-svn ]
    then
        last_reminder=`cat .reminders-svn`
        ((since=now_s-last_reminder))
        if [ "${since}" -lt 3600 ] ; then return ; fi
    fi

    echo -e "${colorboldgreen}                                                                                ${undocolor}"
    echo -e "${colorboldgreen}                   Every time you start work, run svn update                    ${undocolor}"
    echo -e "${colorboldgreen}                   Every time you stop work,  run svn commit                    ${undocolor}"
    echo -e "${colorboldgreen}                                                                                ${undocolor}"
    echo "${now_s}" > .reminders-svn
}

# reminder every 20 minutes - no logbook no marks
reminders_logbook()
{
    if [ -f .reminders-logbook ]
    then
        last_reminder=`cat .reminders-logbook`
        ((since=now_s-last_reminder))
        if [ "${since}" -lt 1200 ] ; then return ; fi
    fi

    echo -e "${colorboldred}                                                                                ${undocolor}"
    echo -e "${colorboldred}                       Remember to write in your LOGBOOK!                       ${undocolor}"
    echo -e "${colorboldred}                              No logbook, No marks!                             ${undocolor}"
    echo -e "${colorboldred}                                                                                ${undocolor}"
    echo "${now_s}" > .reminders-logbook
}

reminders()
{
    now_s=`date +%s`
    reminders_svn
    reminders_logbook
}

# nullglob is set so patterns can be empty
shopt -s nullglob

# create an updates/Makefile if we haven't got one
default_makefile()
{
    if [ ! -f updates/Makefile ]
    then
        echo ".PHONY: _ \$(MAKECMDGOALS)" > updates/Makefile
        echo "\$(eval \$(sort \$(MAKECMDGOALS)):;@:)" >> updates/Makefile
        echo "_:;@:" >> updates/Makefile
        chmod 600 updates/Makefile
    fi
}

# exit with error message - ensure we have a Makefile first ...
exit_with_error()
{
    echo "${now} ${@}" | tee -a updates/log
    default_makefile
    exit -1
}

# tools we check - which, zip, unzip, diff, svn, cksum, java
# check tool, add to list if we cannot find it
declare -a bad_tools
check_tool()
{
    tool="${1}"
    which_tool=`/usr/bin/which "${tool}" 2> /dev/null`

    # giveup if which failed, result is "" or it is not executable
    if [ "$?" -ne 0 -o ${#which_tool} -eq 0 -o ! -x "${which_tool}" ]
    then
        bad_tools[${#bad_tools[@]}]="${tool}"
    fi
}

# check list of tools
check_tools()
{
    # check /usr/bin/which first, its a bit tricky without it
    if [ ! -x '/usr/bin/which' ]
    then
        exit_with_error "Cannot find /usr/bin/which - please install and try again"
    fi

    while [ "${#@}" -gt 0 ]
    do
        check_tool "${1}"
        shift 1
    done
}

# if any tools not found, write out list of bad tools and exit
report_bad_tools()
{
    if ((${#bad_tools[@]} > 0)) ; then
        echo "${now} The following command(s) cannot be found:"
        for ((i = 0 ; i < ${#bad_tools[@]} ; i++ )) ; do
            echo "${now} .  ${bad_tools[$i]}"
        done
        exit_with_error "${now} Please install them and try again."
    fi
}

# function to check C++ compiler and record compiler found
# sets compiler_found if given compiler is acceptable
compiler_found=
check_compiler()
{
    # give up if a compiler already found
    if ((${#compiler_found} > 0)) ; then return ; fi

    compiler="${1}"
    compiler=`/usr/bin/which "${compiler}" 2> /dev/null`

    # giveup if which failed, result is "" or it is not executable
    if [ "$?" -ne 0 -o ${#compiler} -eq 0 -o ! -x "${compiler}" ] ; then return ; fi

    # extract major version number
    version=`"${compiler}" --version | head -1 | cut -f "${version_field}" -d ' '| cut -f 1,1 -d '-'| cut -f 1 -d '.'`

    # if specified, check against minimum acceptable version
    min_version="${2}"
    if [ "${#version}" -gt 0 -a "${#min_version}" -gt 0 ]
    then
        if [ "${version}" -lt "${min_version}" ] ; then return ; fi
    fi

    # if still going, we found an acceptable compiler
    compiler_found="${compiler}"
}

# look for C++ compiler in preferred order ... currently:
# clang++ 9*, 11*, 10*, 8*, 7*, 6*
# g++ 8*, 10*, 9*, 7*
check_compilers()
{
    if [ "${System}" == "Darwin" ]
    then
        version_field=4
        # On a Mac just check clang++ min version is 10.0.0, prefer 11.0.3
        # 11.0.3 => clang 9.0.0
        # 11.0.0 => clang 8.0.0
        # 10.0.1 => clang 7.0.0
        # 10.0.0 => clang 6.0.0
        check_compiler `/usr/bin/which clang++ 2> /dev/null` 10
    else
        version_field=3

        # On our RHEL7 systems clang++-9 is in /opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9
        check_compiler clang++-9  9
        if [ "${#compiler_found}" -eq 0 -a -x "/opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9" ]
        then
            # PATH not set correctly to use our preferred compiler
            echo "${now} PATH does not include /opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9"
            echo "${now} To use this C++ compiler please run this before trying again: source /opt/rh/llvm-toolset-9.0/enable"
        fi
        check_compiler clang++  9
        check_compiler clang++-11  11
        check_compiler clang++-10  10
        check_compiler clang++-8  8
        check_compiler clang++  8
        check_compiler clang++-7  7
        check_compiler clang++  7
        check_compiler clang++-6  6
        check_compiler clang++  6

        # On our RHEL7 systems g++-8 is in /opt/rh/devtoolset-8/root/usr/bin/g++-8
        check_compiler g++-8  8
        if [ "${#compiler_found}" -eq 0 -a -x "/opt/rh/devtoolset-8/root/usr/bin/g++-8" ]
        then
            if [ ! -x "/opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9" ]
            then
                # PATH not set correctly to use our preferred backup compiler
                echo "${now} PATH does not include /opt/rh/devtoolset-8/root/usr/bin/g++-8"
                echo "${now} To use this C++ compiler please run this before trying again: source /opt/rh/devtoolset-8/enable"
            fi
        fi
        check_compiler g++  8
        check_compiler g++-10  10
        check_compiler g++-9  9
        check_compiler g++-7  7
        check_compiler g++  7
    fi

    if [ "${#compiler_found}" -eq 0 ]
    then
        bad_tools[${#bad_tools[@]}]="clang++-9, or clang++ version 6 or later, or g++ version 7 or later"
    fi
}

# check which svn repository directory this working copy belongs too
# give up if this is not a working copy!
repouser=
repoassign=
check_svn()
{
    repo=`svn info 2> /dev/null | grep "^URL:"`
    if [ "${#repo}" -eq 0 ]
    then
        # trying adding . in case we are inside a working copy
        add_dot="X`svn add --depth=empty . 2> /dev/null`"
        if [ "${add_dot:0:2}" != "XA" ]
        then
            exit_with_error "`pwd` is not a working copy!"
        else
            # add worked so recheck svn info
            echo "${now} Added . to svn"

            repo=`svn info 2> /dev/null | grep "^URL:"`
            if [ "${#repo}" -eq 0 ]
            then
                exit_with_error "`pwd` is not a working copy!"
            fi
        fi
    fi

    # remove server from URL, leaving user and path
    repo_path="${repo##URL: https://version-control.adelaide.edu.au/svn/}"
    if [ "${repo}" == "${repo_path}" ]
    then
        exit_with_error "Repository URL must start with: https://version-control.adelaide.edu.au/svn/"
    fi

    # extract user - everything up to first /
    repo_user="${repo_path%%/*}"
    repo_path="${repo_path#*/}"

    # extract year - everything after the first /
    repo_year="${repo_path%%/*}"
    repo_path="${repo_path#*/}"

    # extract sem - everything after the first /
    repo_sem="${repo_path%%/*}"
    repo_path="${repo_path#*/}"

    # extract course - everything after the first /
    repo_course="${repo_path%%/*}"
    repo_path="${repo_path#*/}"

    # extract assign - everything after the first /
    repo_assign="${repo_path%%/*}"
    repo_path="${repo_year}/${repo_sem}/${repo_course}/${repo_assign}"

    echo "${now} SVN user: $repo_user, repository path: $repo_path" >> updates/log
}

declare -a svn_status
declare -a svn_not_found
declare -a svn_not_added
declare -a svn_not_committed
warn_svn_status()
{
    while [ ${#@} -gt 0 ]
    do
        file="${1##./originals/}"
        shift 1

        if [ ! -f "${file}" -a ! -d "${file}" ]
        then
            svn_not_found[${#svn_not_found[@]}]="${file}"
            continue
        fi

        stat=`svn status --depth=empty -v "${file}" 2> /dev/null`
        if [ ${#stat} -eq 0 ]
        then
            svn_not_added[${#svn_not_added[@]}]="${file}"
            continue
        fi

        # check status of
        case "${stat:0:1}" in
        " ")
            # nothing to report
            ;;
        "M" | "R")
            # nothing to report
            ;;
        "A")
            svn_not_committed[${#svn_not_committed[@]}]="${file}"
            ;;
        "I")
            svn_not_added[${#svn_not_added[@]}]="${file}"
            ;;
        "?")
            svn_not_added[${#svn_not_added[@]}]="${file}"
            ;;
        "!")
            svn_status[${#svn_status[@]}]="${file} is missing"
            ;;
        "D")
            svn_not_added[${#svn_not_added[@]}]="${file}"
            ;;
        "C")
            svn_status[${#svn_status[@]}]="${file} has conflicts to be resolved"
            ;;
        "X")
            svn_status[${#svn_status[@]}]="${file} is an external reference to another svn repository"
            ;;
        "~")
            svn_status[${#svn_status[@]}]="${file} has changed type"
            ;;
        *)
            # no idea - just ignore it ...
            ;;
        esac
    done
}

check_svn_originals()
{
    # Makefile, updates/installer.bash, updates/updater.bash and updates/log must all be in svn
    warn_svn_status . Makefile updates updates/installer.bash updates/updater.bash updates/log updates/*-*-*.zip
    # if originals extracted - check if they are in svn
    if [ -d ./originals ] ; then
        warn_svn_status `find ./originals -type f`
    fi

    # files that do not exist
    if ((${#svn_not_found[@]} > 0)) ; then
        echo "${now} The following files / directories could not be found:"
        echo -n "${now}        "
        for ((i = 0 ; i < ${#svn_not_found[@]} ; i++ )) ; do
            echo -n " ${svn_not_found[$i]}"
        done
        echo
    fi

    # files not committed to svn
    if ((${#svn_not_committed[@]} > 0)) ; then
        echo "${now} The following files / directories have not been committed to svn:"
        echo -n "${now}    "
        for ((i = 0 ; i < ${#svn_not_committed[@]} ; i++ )) ; do
            echo -n " ${svn_not_committed[$i]}"
        done
        echo
    fi

    # files not added to svn
    if ((${#svn_not_added[@]} > 0)) ; then
        echo "${now} The following files / directories are not in svn:"
        echo -n "${now}    "
        for ((i = 0 ; i < ${#svn_not_added[@]} ; i++ )) ; do
            echo -n " ${svn_not_added[$i]}"
        done
        echo

        echo "${now} To add the missing files / directories run:"
        echo -n "${now}     svn add --depth=empty"
        for ((i = 0 ; i < ${#svn_not_added[@]} ; i++ )) ; do
            echo -n " ${svn_not_added[$i]}"
        done
        echo
    fi

    # other stuff ...
    if ((${#svn_status[@]} > 0)) ; then
        echo "${now} The following svn issues need to be addressed:"
        for ((i = 0 ; i < ${#svn_status[@]} ; i++ )) ; do
            echo "${now} ${svn_status[$i]}"
        done
    fi
}

# what kind of system?
System=`test -x /usr/bin/uname && /usr/bin/uname -s`
if [ "${#System}" -eq 0 ] ; then System=Linux ; fi

check_tools zip unzip svn cksum diff grep make java find shasum base64
check_compilers | tee -a updates/log
report_bad_tools | tee -a updates/log
check_svn
check_svn_originals

#echo "Found C++ compiler: $compiler_found"

# do we have any update files?
update_zips=(updates/*-*-*.zip)

# give up if no update files
if [ "${#update_zips[@]}" -eq 0 ]
then
    exit_with_error "Cannot find any zip files in updates/"
fi

# name of newest update zip file
update_zip="${update_zips[${#update_zips[@]}-1]}"
update_dir="${update_zip%%.zip}"

# remove quarantine flag if on macos
if [ -x /usr/bin/xattr ]
then
    chmod u+w "${update_zip}"
    xattr -rd com.apple.quarantine "${update_zip}"
fi

# unzip the newest file
unzip -qqud updates "${update_zip}"
chmod -R go= "${update_zip}"

# check we have an update directory
if [ ! -d ${update_dir} ]
then
    exit_with_error "Cannot find directory ${update_dir}"
fi
update_basename=`basename ${update_dir}`

# run update's updater.bash if present, otherwise the generic one
if [ -f "${update_dir}/updater.bash" ]
then
    bash "${update_dir}/updater.bash" "${update_basename}" "${now}" | tee -a updates/log
else
    bash "updates/updater.bash" "${update_basename}" "${now}" | tee -a updates/log
fi

# make sure we have a minimal Makefile
default_makefile

# finally remind students about svn update/svn commit and their logbooks
reminders

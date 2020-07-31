#!/bin/bash

# updater.bash script

# this script should be in work/updates/updater.bash or work/updates/*-*-*/updater.bash
# it creates/updates the work directory structure (inside the student's svn working copy?)

# The contents of the zip file are copied onto the work directory
# . creates and updates are displayed for the student to see
# . Makefile -> copied to work/updates/Makefile
# . README -> copied to work/updates/README
#   . notes about what has changed could be put in here
#     including instructions if anything needs deleted ...
#
# After copying, the exectuables should be made executable
# . changes are displayed to the student
#
# If installation not marked as completed try removing quarantine flags
# . this is only relevant on MacOS where the xattr command exists
# . it is very slow so avoid if possible ...
#
# Finally the originals directory of directories and files that students edit
# are copied into the locations students expect, if they are not there!
# . creates are displayed for students to see
# . if different - originals are not copied - that would destroy the student's work

# updates directory is first arg
zipdir="${1}"

# datestamp to prefix output:
banner="${2} updater: "

# make sure this is run from the working copy
if [ ! -d "updates/${zipdir}" ]
then
    echo "${banner} updater.bash must be run from the working copy directory"
    exit -1
fi

# record working copy and update directories
WORK_DIR="`pwd`"
UPDATES_DIR="${WORK_DIR}/updates"
UPDATE_DIR="${WORK_DIR}/updates/${zipdir}"

# check script is in updates or updates/zipfile
SCRIPT_DIR=`dirname $0`
SCRIPT_DIR=`cd ${SCRIPT_DIR};pwd`
if [ "${SCRIPT_DIR}" != "${WORK_DIR}/updates" -a "${SCRIPT_DIR}" != "${WORK_DIR}/updates/${zipdir}" ]
then
    echo "${banner} updater.bash is in ${SCRIPT_DIR} not ${WORK_DIR}/updates or ${WORK_DIR}/updates/${zipdir}"
    exit -1
fi

# change to zip directory before applying updates
cd "${UPDATE_DIR}"

# installation check - currently to avoid very slow quarantine flag removal on macos
INSTALLED="${UPDATE_DIR}-installed"

# allow patterns that do not match to be empty
shopt -s nullglob

# create directory if it does not exist
create_directory()
{
    src="${1}"
    dst="${2}"
    displayname="${3}"

    # create dir if it does not exist
    if [ ! -d "${dst}" ]
    then
        echo "${banner} Directory: ${displayname}"
        mkdir "${dst}"
    fi

    # directory should be set to 700
    chmod 700 "${dst}"
}

# create matching directories in work - skips .
create_directories()
{
    while [ "${#@}" -gt 0 ] ; do
        src="${1}"
        dst="${WORK_DIR}/${1}"
        shift 1

        if [ "${src}" == "." -o "${src}" == "./updates" ] ; then continue ; fi

        create_directory "{$src}" "${dst}" "${src}"
    done
}

# create file if it does not exist, if it does update on request
create_file()
{
    src="${1}"
    dst="${2}"
    displayname="${3}"
    or_update="${4}"

    # create file if it does not exist
    if [ ! -f "${dst}" ]
    then
        echo "${banner} Creating: ${displayname}"
        cp -p "${src}" "${dst}"
        chmod 600 "${dst}"

    elif  [ "${or_update}" == "update" ]
    then
        # if file is out of date, update it
        if ! cmp "${src}" "${dst}" > /dev/null 2> /dev/null ;
        then
            echo "${banner} Updating: ${displayname}"
            cp "${src}" "${dst}"
        fi
    fi
}

# create / update files
create_files()
{
    while [ "${#@}" -gt 0 ] ; do
        src="${1#}"
        dst="${WORK_DIR}/${1}"
        displayname="${src}"
        shift 1

        # copy the Makefile and README to updates/Makefile or udpates/README respectively
        if [ "${src}" == "./Makefile" -o "${src}" == "./README" ]
        then
            dst="${UPDATES_DIR}/${src#./}"
            displayname="updates/${src#./}"
        fi

        # do not copy the updater.bash script
        if [ "${src}" != "./updater.bash" ]
        then
            create_file "${src}" "${dst}" "${displayname}" update
        fi
    done
}

# now remove quarantine flags
fix_quarantine()
{
    if [ -x /usr/bin/xattr ]
    then
        echo "${banner} Removing MacOS quarantine flags from the executables"
        xattr -rd com.apple.quarantine ${@}
    fi
}

# now fix up the file permissions ...
fix_executable()
{
    while [ "${#@}" -gt 0 ] ; do
        src="${1}"
        dst="${WORK_DIR}/${1}"
        shift 1

        # ignore files obviously not exectuable, .cpp, .h, .tst, .cmp, .hdl, ...
        case "${src}" in
        *.cpp | *.h | *.tst | *.cmp | *.hdl | *.jack | *.php | *.c | *.o | *.a)
            ;;
        *)
            # ignore anything that is not a file or is already executable
            if [ ! -f "${dst}" -o -x "${dst}" ] ; then continue ; fi

            # ignore the Makefile and README
            if [ "${src}" == "Makefile" -o "${src}" == "README" ] ; then continue ; fi

            # set executables to be 700
            echo "${banner} Made ${src} executable"
            chmod 700 "${dst}"
            ;;
        esac
    done
}

# now fixup the startup files and directories in originals
fix_startup_directories()
{
    while [ "${#@}" -gt 0 ] ; do
        src="${1}"
        displayname="${src##./originals/}"
        shift 1

        # ignore root orginals directory
        if [ "${#displayname}" -eq 0 ] ; then continue ; fi

        dst="${WORK_DIR}/${displayname}"
        create_directory "${src}" "${dst}" "${displayname}"
    done
}

fix_startup_files()
{
    while [ "${#@}" -gt 0 ] ; do
        src="${1}"
        displayname="${src##./originals/}"
        dst="${WORK_DIR}/${displayname}"
        shift 1

        create_file "${src}" "${dst}" "${displayname}" nochange
    done
}

# create directories and create / update files
create_directories `find . -type d`
create_files `find . -type f`

# make sure executables are executable and not quarantined
fix_executable * bin/* lib/*/bin/*

# if not previously installed - check qurantine flags
if [ ! -f "${INSTALLED}" ]
then
    fix_quarantine * bin/* lib/*/bin/*
fi

# copy original startups so students can see them - if they do not exist yet!
if [ -d originals ]
then
    fix_startup_directories `find ./originals -type d`
    fix_startup_files `find ./originals -type f`
fi

# finally touch the installed file so we do not waste time!
if [ ! -f "${INSTALLED}" ]
then
    touch "${INSTALLED}"

    # make sure we have a README
    if [ ! -f README ]
    then
        echo "${banner} Updating: updates/README"
        echo "${banner} Sorry, no update details were provided with ${zipdir}!" > "${UPDATES_DIR}/README"
        echo "${banner} Update installed"
    else
        echo "${banner} Update installed - see updates/README for details"
    fi
fi

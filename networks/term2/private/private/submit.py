#!/usr/bin/env python3
import subprocess
import glob
import json
import sys
import os
import os.path
import errno
import shutil

SHADOW_REPO_DIR="../.submit-repo"
VERBOSE=False

def git(*args):
    if VERBOSE:
        print("> {}".format(" ".join(["git"] + list(args))))
        subprocess.check_call(" ".join(["git"] + list(args)), cwd=SHADOW_REPO_DIR, shell=True)
    else:
        subprocess.check_output(["git"] + list(args), cwd=SHADOW_REPO_DIR, stderr=subprocess.PIPE)

def git_output(*args):
    if VERBOSE:
        print("> {}".format(" ".join(["git"] + list(args))))

    return subprocess.check_output(["git"] + list(args)).strip().decode('utf-8')

def set_up_shadow_repo(task_name, user_name, user_email):
    try:
        shutil.rmtree(SHADOW_REPO_DIR)
    except FileNotFoundError:
        pass
    os.makedirs(SHADOW_REPO_DIR)

    git("init")
    git("remote", "add", "local", "..")
    git("fetch", "local", "main".format(task_name))


    git("checkout", "local/main")
    student_repo_url = git_output("remote", "get-url", "student")
    git("remote", "add", "student", student_repo_url)

    with open(os.path.join(SHADOW_REPO_DIR, ".git", "config"), mode="a", encoding="utf-8") as config:
        config.write("[user]\n\tname = {}\n\temail = {}\n".format(user_name, user_email))

def create_commits(task_name, files):
    git("checkout", "-b", "submits/" + task_name)

    try:
        os.makedirs(os.path.join(SHADOW_REPO_DIR, task_name))
    except:
        pass
    
    for filename in files:
        directory = os.path.join(SHADOW_REPO_DIR, task_name, os.path.dirname(filename))
        for path in glob.glob(filename):
            try:
                os.makedirs(directory)
            except:
                pass
            shutil.copy(path, directory)

        git("add", task_name + "/" + filename)

    git("commit", "-m", task_name, "--allow-empty")

def push_branches(task_name):
    git("push", "-f", "local", "submits/" + task_name)
    git("push", "-f", "student", "submits/" + task_name)

def ensure_list(value):
    if not isinstance(value, list):
        return [value]
    return value

if __name__ == "__main__":
    if "-v" in sys.argv:
        VERBOSE=True
    
    try:
        task_config = json.load(open(".tester.json"))
    except FileNotFoundError:
        print("error: Task config not found. Are you running tool from correct directory?")
        exit(1)

    user_name = git_output("config", "user.name")
    user_email = git_output("config", "user.email")

    task_name = os.path.basename(os.path.realpath("."))
    set_up_shadow_repo(task_name, user_name, user_email)

    create_commits(task_name, ensure_list(task_config["allow_change"]))

    push_branches(task_name)

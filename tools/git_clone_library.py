#Import("env")
from git import Repo  # pip install gitpython

git_arguments = input("Enter library name: ")
#git_arguments = env.GetProjectOption("git_arguments", "")
git_url = "https://github.com/Nietaktowny/wifi_controller.git"

Repo.clone_from(git_url, git_arguments)

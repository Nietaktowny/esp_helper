#! /usr/bin/python3


#Import("env")
import os
import tempfile
import shutil
import git
from git import RemoteProgress
from tqdm import tqdm

user_input = input("Enter libraries name: ")
git_url = "https://github.com/Nietaktowny/esp_helper.git"
lib = user_input.split()

class CloneProgress(RemoteProgress):
    def __init__(self):
        super().__init__()
        self.pbar = tqdm()

    def update(self, op_code, cur_count, max_count=None, message=''):
        self.pbar.total = max_count
        self.pbar.n = cur_count
        self.pbar.refresh()

# Clear old versions of libraries
if os.path.isdir("lib"):
    shutil.rmtree("lib")
# Create temporary dir
t = tempfile.mkdtemp()
# Clone into temporary dir
git.Repo.clone_from(git_url, t, progress=CloneProgress())
# Copy desired files from temporary dir
for file in lib:
    shutil.move(os.path.join(t, 'libraries/' + file), 'lib/' + file)
# Remove temporary dir
shutil.rmtree(t)
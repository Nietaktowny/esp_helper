Import("env")
import subprocess
import os

def linter_callback(*arg, **kwargs):
    print("Executing Flawfinder")
    
    flawfinder_arguments = env.GetProjectOption("flawfinder_arguments", "")
    os.chdir("docs")

    env.Execute("flawfinder -H" + " ../lib" + "  " + flawfinder_arguments + " . > flawfinder/flawfinder.html")

env.AddCustomTarget(
    "flawfinder",
    None,
    linter_callback,
    title="flawfinder",
    description="Executes Flawfinder")
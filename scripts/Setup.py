import os
import subprocess
import CheckPython

# Make sure everything we need is installed
CheckPython.ValidatePackages()

import Vulkan

# Change from Scripts directory to root of project
os.chdir("../")

if (not Vulkan.CheckVulkanSDK()):
	print("Vulkan SDK not installed.")
	exit(1)

if (not Vulkan.CheckVulkanSDKDebugLibs()):
	print("Vulkan SDK debug libraries not found.")
	exit(1)

print("\nUpdating submodules...")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

print("Running Premake5...")
subprocess.call(["extern/premake/bin/premake5.exe", "vs2022"])

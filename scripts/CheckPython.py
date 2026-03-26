import subprocess
import pkg_resources

def install(package):
	print(f"Installing {package} module...")
	subprocess.check_call(["python", "-m", "pip", "install", package])

def ValidatePackage(package):
	required = { package }
	installed = {pkg.key for pkg in pkg_resources.working_set}
	missing = required - installed
	if missing:
		print(f"Missing packages: {', '.join(missing)}")
		for pkg in missing:
			install(pkg)
	else:
		print(f"All required packages are installed.")

def ValidatePackages():
	ValidatePackage("requests")
	ValidatePackage("fake-useragent")

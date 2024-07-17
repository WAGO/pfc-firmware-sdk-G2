#!/usr/bin/env python3

import subprocess
import os
import shutil
from dataclasses import dataclass

@dataclass
class Package:
    name: str
    src_dir: str
    version: str = None
    patches: str = None
    rule_file: str = None
    menu_file: str = None

class PTXdist:
    def __init__(self):
        pass

    def __run(self, command, fail_on_error=True):
        result = subprocess.run(command, capture_output=True, text=True, check=False)
        if fail_on_error and result.returncode != 0:
            print(f"PTXdist: failed to invoke: {command}")
            print(f"  EXIT_CODE: {result.returncode}")
            print(f"  STDOUT:    {result.stdout}")
            print(f"  STDERR:    {result.stderr}")
            raise Exception(f"PTXdist: failed to invoke: {command}")
        return result.stdout

    def print(self, name):
        command = ['ptxdist', '-k', 'print']
        if isinstance(name, str):
            command.append(name)
        else:
            command.extend(name)
        return self.__run(command)

    def package_info(self, name, fail_on_error=True):
        command = ['ptxdist', '-k', 'package-info']
        if isinstance(name, str):
            command.append(name)
        else:
            command.extend(name)
        return self.__run(command, fail_on_error)

    def list_packages(self):
        package_vars = ['PACKAGES-y', 'PACKAGES-m', 'LAZY_PACKAGES-y',
                        'HOST_PACKAGES-y', 'HOST_PACKAGES-m', 'IMAGE_PACKAGES-y']
        return self.print(package_vars).split()

    def go(self):
        command = ['ptxdist', 'go', '-q', '-j', '-n19']
        self.__run(command)

    def images(self):
        command = ['ptxdist', 'images', '-q', '-j', '-n19']
        self.__run(command)

def base_var(name):
    name = name.upper()
    name = name.replace('-', '_')
    return name


class PackageList:
    """Contains a list of all packages."""
    def __init__(self, ptxdist=PTXdist(), add_package_info=False):
        package_names = ptxdist.list_packages()
        self.__init_packages(ptxdist, package_names)
        if add_package_info:
            self.__add_package_info(ptxdist, package_names)

    def __init_packages(self, ptxdist, package_names):
        src_dirs = self.__get_package_properties(ptxdist, "SRC_DIR", package_names)
        srcs = self.__get_package_properties(ptxdist, "SRC", package_names)
        urls = self.__get_package_properties(ptxdist, "URL", package_names)
        self.packages = {}
        for i, name in enumerate(package_names):
            if src_dirs[i] != '':
                src_dir = os.path.abspath(src_dirs[i])
            elif urls[i].startswith('file://'):
                src_dir = os.path.abspath(urls[i][len('file://'):])
            elif urls[i].startswith('lndir://'):
                src_dir = os.path.abspath(urls[i][len('lndir://'):])
            elif srcs[i] != '':
                src_dir = os.path.abspath(srcs[i])
            else:
                src_dir = None
                print(f"info: missing sources of package \"{name}\"")
            package = Package(name, src_dir)
            self.packages[name] = package

    def __add_package_info(self, ptxdist, package_names):
        package = None
        package_info = ptxdist.package_info(package_names, fail_on_error=False)
        for line in package_info.splitlines():
            if line.startswith('package:'):
                package_name = line[len('package:'):].strip()
                package = self.packages.get(package_name, None)

            if package is not None:
                if line.startswith('version:'):
                    package.version = line[len('version:'):].strip()
                elif line.startswith('rule file:'):
                    package.rule_file = line[len('rule file:'):].strip()
                elif line.startswith('menu file:'):
                    package.menu_file = line[len('menu file:'):].strip()
                elif line.startswith('patches:'):
                    package.patches = line[len('patches:'):].strip()

    def __get_package_properties(self, ptxdist, property_name, package_names):
        var_names = [f"{base_var(name)}_{property_name}" for name in package_names]
        return ptxdist.print(var_names).split('\n')

    def get_src_dirs(self):
        src_dirs = []
        for package in self.packages.values():
            if package.src_dir is not None:
                src_dirs.append(package.src_dir)
        return src_dirs

    def get_patch_dirs(self):
        patch_dirs = []
        for package in self.packages.values():
            if package.patches is not None:
                patch_dirs.append(package.patches)
        return patch_dirs

def remove_unused_files_and_directories(directory, dirs_to_keep):
    print(f"remove unused files and directories in \"{directory}\":")
    for filename in os.listdir(directory):
        path = os.path.abspath(os.path.join(directory, filename))
        if os.path.isdir(path):
            if not path in dirs_to_keep:
                print(f"remove {path}")
                shutil.rmtree(path)
        else:
            print(f"remove {path}")
            os.unlink(path)

def build():
    packages = PackageList(add_package_info=False)
    dirs_to_keep = packages.get_src_dirs()
    for directory in ["src", "local_src"]:
        remove_unused_files_and_directories(directory, dirs_to_keep)

    ptxdist = PTXdist()
    ptxdist.go()
    ptxdist.images()

if __name__ == "__main__":
    build()

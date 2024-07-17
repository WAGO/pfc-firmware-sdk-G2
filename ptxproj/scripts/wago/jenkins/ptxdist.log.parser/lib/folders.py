from __future__ import annotations
from collections import namedtuple
from functools import lru_cache
import os.path
from typing import Dict, Tuple


Folder = namedtuple("Folder", ["path", "module"])


class Folders:
    DEFAULT_MAPPINGS: Dict[str, str] = {
        "/workspace/pfc/": "",
        "platform-": "ptxproj/platform-",
    }
    LOOKBEHIND_ELEMENT_COUNT: int = 100
    LOOKFORWARD_ELEMENT_COUNT: int = LOOKBEHIND_ELEMENT_COUNT * 100

    # pylint: disable=dangerous-default-value
    # We use a mutable dict that could possibly be mutated in consecutive calls.
    def __init__(
        self,
        build_folder: str = ".",
        work_folder: str = ".",
        default_package: str = "",
        mappings=None,
    ):
        self.folders = [Folder("", default_package)]
        self.build_folder = build_folder
        self.work_folder = work_folder
        self.mappings = mappings or self.DEFAULT_MAPPINGS

    def apply_mapping(self, path: str) -> str:
        """
        If path starts with any of mappings.key, replace it with mappings.value
        """
        for key, value in self.mappings.items():
            if path.startswith(key):
                return path.replace(key, value)
        return path

    def len(self) -> int:
        return len(self.folders)

    def append_folder(self, path: str, package: str) -> Folders:
        self.folders.append(Folder(path, package))
        return self

    def map_and_append_folder(self, path: str, package: str) -> Folders:
        self.append_folder(self.apply_mapping(path), package)
        return self

    def prepend_build_folder(self, file_name: str) -> str:
        if not self.is_absolute_path(file_name):
            file_name = os.path.normpath(os.path.join(self.build_folder, file_name))
        return file_name

    @lru_cache(maxsize=LOOKFORWARD_ELEMENT_COUNT)
    def exists(self, path: str) -> bool:
        return os.path.exists(os.path.join(self.work_folder, self.build_folder, path))

    def resolve_file_name(self, path: str, folder: str = "", idx: int = -1) -> Tuple[str, str]:
        path = os.path.normpath(path)
        if self.is_absolute_path(path):
            return self.apply_mapping(path), self.folders[idx - 1].module
        path = self.apply_mapping(path)
        if not folder:
            nearby_folders = (
                self.folders[idx - 1 : idx - self.LOOKBEHIND_ELEMENT_COUNT : -1]
                + self.folders[idx : idx + self.LOOKFORWARD_ELEMENT_COUNT : +1]
            )
        else:
            nearby_folders = [Folder(folder, "")]
        for fdr, pkg in nearby_folders:
            candidate = os.path.normpath(os.path.join(fdr, path))
            if self.exists(candidate):
                return candidate, pkg
        return (
            path,
            nearby_folders[0].module,
        )

    @staticmethod
    def is_absolute_path(name: str):
        """
        check a given path is absolute
        """
        return name.startswith("/")

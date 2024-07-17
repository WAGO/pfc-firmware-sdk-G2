from __future__ import annotations
from dataclasses import dataclass, field, asdict
from enum import Enum
from functools import total_ordering
import json

from lib.folders import Folders


@total_ordering
class Severity(Enum):
    NORMAL = "NORMAL"
    ERROR = "ERROR"
    LOW = "LOW"

    def __lt__(self, other):
        return (self == Severity.LOW and other != Severity.LOW) or (other == Severity.ERROR and self != Severity.ERROR)


@dataclass(order=True, unsafe_hash=True)
class Issue:
    """Class holding the information to display a single issue."""

    # pylint: disable=too-many-instance-attributes

    # pylint: disable=invalid-name
    folder_idx: int = field(repr=False, hash=False, compare=False, default=0)
    fileName: str = "-"
    lineStart: str = "0"
    columnStart: str = "0"
    category: str = ""
    severity: Severity = Severity.NORMAL
    message: str = ""
    moduleName: str = ""

    def __init__(self, iterable=(), **kwargs) -> None:
        self.__dict__.update(iterable, **kwargs)
        if isinstance(self.severity, str):
            self.severity = Severity[self.severity]
        self.fileName = self.fileName or "-"
        self.lineStart = self.lineStart or "0"
        self.columnStart = self.columnStart or "0"
        self.severity = self.severity or Severity.NORMAL

    def resolve_file_name(self, folders: Folders) -> Issue:
        """
        Resolve this issue's fileName against some known folders to find the complete path.
        """
        self.fileName, self.moduleName = folders.resolve_file_name(self.fileName, idx=self.folder_idx)
        if folders.exists(self.fileName):
            self.fileName = folders.prepend_build_folder(self.fileName)
        return self


class IssueEncoder(json.JSONEncoder):
    """Class used to encode an issues as JSON."""

    def default(self, o):
        # pylint: disable=method-hidden
        if isinstance(o, Issue):
            return {k: v for k, v in asdict(o).items() if k not in ["folder_idx"]}
        if isinstance(o, Severity):
            return o.value
        # Base class default() raises TypeError:
        return super().default(o)

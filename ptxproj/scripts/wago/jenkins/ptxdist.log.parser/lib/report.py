from __future__ import annotations
import os
import re
import sys
from typing import Generator, List, Optional, Set, TextIO
from lib.folders import Folders
from lib.issue import Issue, Severity
from lib.logging import log


def first(iterable):
    for element in iterable:
        if element:
            return element
    return None


class Report:
    # pylint: disable=line-too-long
    # pylint: disable=invalid-name
    # autopep8: off
    IGNORED_FILES = ["configure.ac", "configure.in", "Makefile.am"]
    # some message patterns resolving to a low priority grade.
    LOW_PRIORITY_PATTERNS = [
        r".* is not documented.$",  # message from Doxygen
        r"^explicit link request to .* could not be resolved$",  # message from Doxygen
        r".* has become obsolete.",  # message from Doxygen
    ]

    PACKAGE_REGEX = re.compile(
        r"^(target:) (?P<package>.*)\.(get|extract|prepare|compile|install|targetinstall)(\.(pre|post))?$"
    )
    ENTERING_REGEX = re.compile(r"^\w*(?:\[\d*\])?:\s*(Entering)\s*directory\s*['`](?P<directory>.*)['`]$")
    WARNINGS_PATTERNS = [
        # WARNINGS_REGEX
        r"^(?:.* )?(?P<fileName>.+?):(?P<lineStart>\d+):(?:(?P<columnStart>\d+):)? (?P<category>[wW]arning|.*[eE]rror):\s*(?P<message>.*?)\s*$",
        # DOXYGEN_OBSOLETE_WARNING_REGEX
        r"^(?P<category>[Ww]arning): (?P<message>Tag '.*?' at line (?P<lineStart>.*) of file '(?P<fileName>.*?)' has become obsolete.)$",
        # CCWARNING_REGEX
        r"^cc1plus:\s*(?P<category>warning|.*error):\s+(?P<message>.*?)$",
        # MAKE_ERROR_REGEX
        r"^(?P<message>\w.*:.*\[.*\] Error.*)(?<!\(ignored\))$",
    ]
    CONTINUATION_MAX_LOOKAHEAD_LINES = 50
    CONTINUATION_REGEX = re.compile(r"^[\s\d]{4,}[\|+<>].*$|^.* (?:events?|note:|required from|instantiation) .*$")
    # autopep8: on
    warnings_filters = [re.compile(it) for it in WARNINGS_PATTERNS]
    low_priority_filters = [re.compile(it) for it in LOW_PRIORITY_PATTERNS]

    CLASS_REGEX = re.compile(r"\[(?:-W)?(?P<switch>[^\[]+)\]$")

    def __init__(
        self,
        build_folder: str = os.getcwd(),
        work_folder: str = ".",
        default_module: str = "",
        mappings=None,
        multiline=False,
    ) -> None:
        self.package = default_module or ""
        self.build_folder = os.path.relpath(os.path.abspath(build_folder), os.path.abspath(work_folder))
        self.folders = Folders(self.build_folder, work_folder, self.package, mappings=mappings)
        self.multiline = multiline

    def process_line(self, lines: List[str], lineNo: int) -> Optional[Issue]:
        text = lines[lineNo]
        if "target: " in text:
            self.parse_package(text)
        elif "Entering " in text:
            self.parse_folder(text)
        elif "warning: " in text or "error: " in text or "Warning: " in text or "Error: " in text or " Error " in text:
            return self.parse_issue(text, lines, lineNo)
        return None

    def parse_issue(self, text: str, lines: List[str], lineNo: int) -> Optional[Issue]:
        result = first(re.match(filter, text) for filter in self.warnings_filters)
        if result is None:
            return None

        elements = result.groupdict()
        if os.path.basename(elements.get("fileName", "-")) in self.IGNORED_FILES:
            return None

        message = elements["message"]
        if "category" in elements:
            category = elements["category"].capitalize()
            severity = Severity.ERROR if "error" in category.lower() else Severity.NORMAL
            if any(re.match(pattern, message) for pattern in self.low_priority_filters):
                severity = Severity.LOW
        else:
            category = "Error"
            severity = Severity.ERROR

        classMatcher = re.search(self.CLASS_REGEX, message)
        if classMatcher is not None:
            category += ":" + classMatcher.group("switch")

        if self.multiline:
            for nextline in lines[lineNo + 1 : lineNo + self.CONTINUATION_MAX_LOOKAHEAD_LINES]:
                if re.match(self.CONTINUATION_REGEX, nextline):
                    message += "\n" + nextline.replace("\n", "")
                else:
                    break
        elements["message"] = message

        return Issue(
            elements,
            category=category,
            severity=severity,
            moduleName=self.package,
            folder_idx=self.folders.len(),
        )

    def parse_folder(self, text: str) -> None:
        result = re.search(self.ENTERING_REGEX, text)
        if result is None:
            return
        folder = result.group("directory") + "/"
        folder = self.folders.apply_mapping(folder)
        self.folders.append_folder(folder, self.package)

    def parse_package(self, text: str) -> None:
        result = re.search(self.PACKAGE_REGEX, text)
        if result is None:
            return
        self.package = result.group("package")
        self.folders.append_folder(".", self.package)

    @staticmethod
    def progressbar(it, prefix: str = "", size: int = 60, out: TextIO = sys.stdout) -> Generator:
        count = len(it)

        def show(x) -> None:
            log(
                f"{prefix}[{'█'*x}{('.'*(size-x))}] {int(x*count/size):,}/{count:,}",
                end="\r",
                file=out,
                flush=True,
                verbose=2,
            )

        show(0)
        y = -1
        for i, item in enumerate(it):
            yield item
            x = int(size * i / count) + 1
            if x != y:
                y = x
                show(x)
        log("", flush=True, file=out, verbose=2)

    def process_text(self, text: List[str], line_limit: Optional[int] = None) -> Set[Issue]:
        # pass 1
        # raw set with not yet resolved file paths
        raw_issues: Set[Issue] = {
            issue
            for lineNo in self.progressbar(range(len(text[0:line_limit])), "Parsing logfile      ")
            if (issue := self.process_line(text, lineNo)) is not None
        }
        # pass 2
        issues: Set[Issue] = {
            issue.resolve_file_name(self.folders) for issue in self.progressbar(raw_issues, "Resolving file names ")
        }
        return issues

    def process_file(self, file: TextIO, line_limit: Optional[int] = None) -> Set[Issue]:
        return self.process_text(file.readlines(), line_limit=line_limit)

from io import IOBase
import json
from typing import Dict, List, Set, Union

from lib.issue import Issue, IssueEncoder


class JsonReport:
    """
    Read and write the warnings report from/to JSON.
    """

    @staticmethod
    def to_json(issues: Set[Issue]) -> str:
        items: Dict[str, Union[str, List[Issue], int]] = {}
        items["_class"] = "io.jenkins.plugins.analysis.core.restapi.ReportApi"
        items["issues"] = sorted(issues)
        items["size"] = len(issues)
        return json.dumps(items, cls=IssueEncoder, indent=2, ensure_ascii=False)

    @staticmethod
    def write_to_json(issues: Set[Issue], file: IOBase) -> None:
        file.write(JsonReport.to_json(issues))

    @staticmethod
    def from_json(text: str) -> Set[Issue]:
        data = json.loads(text)
        return {Issue(it) for it in data["issues"]}

    @staticmethod
    def read_from_json(file_name: str) -> Set[Issue]:
        with open(file_name, "r", encoding="UTF-8") as handle:
            data = json.load(handle)
        return {Issue(it) for it in data["issues"]}

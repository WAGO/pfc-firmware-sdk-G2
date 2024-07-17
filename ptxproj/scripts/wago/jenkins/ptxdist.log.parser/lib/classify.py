from __future__ import annotations
from collections import namedtuple
from typing import Dict, List, Set
import csv

from lib.issue import Issue


Grouping = namedtuple("Grouping", ["module", "group"])


class Classification:
    """Split a report file (JSON) into several files.

    The issues are grouped according to rules provided by a separate table.
    """

    classification: Dict[str, List[str]]
    """
    key: classification
    value: list of module names
    """

    all_classified_packages: Set[str]

    def __init__(self):
        self.classification = {}
        self.all_classified_packages = set()

    def get_classification_from_csv(
        self, file_name: str, module_name_column: int, classification_column: int
    ) -> Classification:
        """Retrieve classification information from a csv file

        The first line of the table is ALWAYS interpreted as table header and thus discarded.
        There are two columns inside the table which are interesting - the one containing
        a module's name and one containing a classification string.
        An example may be `libwagosnmp:WAGO`.
        """
        with open(file_name, newline="", encoding="UTF-8") as handle:
            reader = csv.reader(handle, delimiter=";")
            table = [Grouping(row[module_name_column], row[classification_column]) for row in list(reader)[1:]]
            self.all_classified_packages = {it.module for it in table}
            classifications = {it.group for it in table}
            self.classification = {
                classifier: [it.module for it in table if it.group == classifier] for classifier in classifications
            }
        return self

    def group_issues(self, issues: Set[Issue]) -> Dict[str, Set[Issue]]:
        """Group the issues according to a previously loaded mapping

        Returns a dictionary with an entry for each classification key.
        The values contain sets of the filtered issues.
        """
        if self.classification is None:
            return None
        all_packages = {issue.moduleName for issue in issues}
        all_unclassified_packages = list(all_packages - self.all_classified_packages)
        all_classifications = self.classification
        all_classifications.update({"unclassified": all_unclassified_packages})
        return {
            group: {issue for issue in issues if issue.moduleName in packages}
            for group, packages in all_classifications.items()
        }

# pylint: disable=line-too-long,missing-module-docstring,missing-class-docstring
# pylint: disable=missing-function-docstring,too-many-public-methods

import unittest
import re


class TestPatterns(unittest.TestCase):
    def read_error_patterns(self, severity='error'):
        with open('project-log-parser-rules.txt', encoding='UTF-8') as rules_file:
            lines = [ line for line in rules_file.read().splitlines() if line.startswith(severity) ]

        return [ re.search('/(.*)/', line).group(1) for line in lines ]


    def read_expectations(self, filename):
        with open('test/' + filename, encoding='UTF-8') as detected_file:
            return [ line for line in detected_file.read().splitlines() if line and not line.startswith('#') ]


    def setup(self, expectations_name, **kwargs):
        # pylint: disable=invalid-name
        self.maxDiff = None
        error_patterns = self.read_error_patterns(**kwargs)
        expected_messages = self.read_expectations(expectations_name)
        matches = { message for message in expected_messages for pattern in error_patterns if re.search(pattern, message) }
        return matches, expected_messages


    def test_all_expected_errors_are_matched(self):
        matches, expected_messages = self.setup('errors_match.txt')

        self.assertCountEqual(expected_messages, matches, "Not all expected messages are found")


    def test_all_unexpected_errors_are_filtered(self):
        matches, _ = self.setup('errors_dont_match.txt')

        self.assertCountEqual(matches, [], "There are unexpected matches")


    def test_which_regex_find_unexpected_errors(self):
        # pylint: disable=invalid-name
        self.maxDiff = None
        error_patterns = self.read_error_patterns()
        expected_messages = self.read_expectations('errors_dont_match.txt')
        for pattern in error_patterns:
            matches = { message for message in expected_messages if re.search(pattern, message) }
            self.assertCountEqual(matches, [], f"Pattern '{pattern}' matches some of the unexpected lines")


    def test_which_regex_lacks_expected_errors(self):
        # pylint: disable=invalid-name
        self.maxDiff = None
        error_patterns = self.read_error_patterns()
        expected_messages = self.read_expectations('errors_match.txt')
        for pattern in error_patterns:
            matches = { message for message in expected_messages if re.search(pattern, message) }
            self.assertNotEqual(matches, set(), f"Pattern '{pattern}' does not match any of the expected messages")


    def test_all_expected_warnings_are_matched(self):
        matches, expected_messages = self.setup('warnings_match.txt', severity='warning')

        self.assertCountEqual(expected_messages, matches, "Not all expected messages are found")


if __name__ == "__main__":
    unittest.main()

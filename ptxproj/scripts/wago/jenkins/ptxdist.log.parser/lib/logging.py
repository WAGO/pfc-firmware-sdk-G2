SELECTED_LOGLEVEL: int = 0


def select_loglevel(level: int) -> None:
    global SELECTED_LOGLEVEL  # pylint: disable=global-statement
    SELECTED_LOGLEVEL = level


def log(*args, **kwargs) -> None:
    argmap = kwargs
    min_verbose_level = argmap.pop("verbose")
    if SELECTED_LOGLEVEL >= min_verbose_level:
        print(*args, **argmap)

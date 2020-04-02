def Settings( **kwargs ):
    return {
            'flags': [
                '-x', 'c++',
                '-Wall', '-Wextra', '-Werror',
                '-I', '.',
                '-I', './bazel-optimizationtools/external/json/single_include',
                ],
            }


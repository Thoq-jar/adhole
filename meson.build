project('adweb', 'c',
        version : '0.1.0',
        default_options : ['warning_level=3', 'c_std=c2x'])

cc = meson.get_compiler('c')
if cc.get_id() != 'clang'
    error('This project must be built with Clang!')
endif

srcs = [
    'src/blocklist.c',
    'src/config.c',
    'src/dns_server.c',
    'src/dns_string.c',
    'src/logger.c',
    'src/main.c',
]

inc = include_directories('include')

executable('adweb', srcs, include_directories : inc)
